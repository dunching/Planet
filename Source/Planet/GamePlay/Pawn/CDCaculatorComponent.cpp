
#include "CDcaculatorComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "CharacterTitle.h"
#include "HumanAIController.h"
#include "PlanetControllerInterface.h"
#include "PlanetPlayerState.h"
#include "HoldingItemsComponent.h"
#include "ItemProxy_Container.h"
#include "GameOptions.h"
#include "SceneProxyTable.h"
#include "ItemProxy_Minimal.h"
#include "Planet.h"
#include "LogWriter.h"

FName UCDCaculatorComponent::ComponentName = TEXT("CDCaculatorComponent");

FSkillCooldownHelper::FSkillCooldownHelper()
{

}

bool FSkillCooldownHelper::CheckCooldown() const
{
	if (CooldownTime > 0.f)
	{
		return CooldownConsumeTime > CooldownTime;
	}
	else
	{
		return true;
	}
}

void FSkillCooldownHelper::IncreaseCooldownTime(float DeltaTime)
{
	CooldownConsumeTime += DeltaTime;
}

void FSkillCooldownHelper::AddCooldownConsumeTime(float NewTime)
{
	IncreaseCooldownTime(NewTime);
}

void FSkillCooldownHelper::FreshCooldownTime()
{
	CooldownConsumeTime = CooldownTime + UGameOptions::GetInstance()->ResetCooldownTime;
}

void FSkillCooldownHelper::ResetCooldownTime()
{
	CooldownConsumeTime = 0.f;
}

void FSkillCooldownHelper::OffsetCooldownTime()
{
	CooldownConsumeTime = CooldownTime - UGameOptions::GetInstance()->ResetCooldownTime;
}

void FSkillCooldownHelper::SetCooldown(float CooldDown)
{
	CooldownTime = CooldDown;
}

bool FSkillCooldownHelper::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	if (CooldownTime < 0.f)
	{
		const auto Remaining = CooldownTime - CooldownConsumeTime;

		if (Remaining <= 0.f)
		{
			RemainingCooldown = 0.f;

			RemainingCooldownPercent = 1.f;

			return true;
		}
		else
		{
			RemainingCooldown = Remaining;

			RemainingCooldownPercent = RemainingCooldown / UGameOptions::GetInstance()->ResetCooldownTime;

			return false;
		}
	}

	const auto Remaining = CooldownTime - CooldownConsumeTime;

	if (Remaining <= 0.f)
	{
		RemainingCooldown = 0.f;

		RemainingCooldownPercent = 1.f;

		return true;
	}
	else
	{
		RemainingCooldown = Remaining;

		RemainingCooldownPercent = RemainingCooldown / CooldownTime;

		return false;
	}
}

UCDCaculatorComponent::UCDCaculatorComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 10;

	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UCDCaculatorComponent::TickComponent(
	float DeltaTime,
	enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto CaculationCooldownLambda = [DeltaTime, this](auto& CooldownMap) {
			auto TempMap = CooldownMap;
			for (auto& Iter : TempMap)
			{
				Iter.Value->IncreaseCooldownTime(DeltaTime);
				if (Iter.Value->CheckCooldown())
				{
					CooldownMap.Remove(Iter.Key);
					CD_FASI_Container.RemoveItem(Iter.Value);
					continue;
				}
				CD_FASI_Container.UpdateItem(Iter.Value);
			}
			};

		CaculationCooldownLambda(Separate_Map);
		CaculationCooldownLambda(Common_Map);
	}
#endif
}

void UCDCaculatorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CD_FASI_Container, Params);
}

void UCDCaculatorComponent::ApplyCooldown(FActiveSkillProxy* ActiveSkillProxyPtr)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 独立CD
		{
			auto SkillCooldownInfoMap = ActiveSkillProxyPtr->GetTableRowProxy_ActiveSkillExtendInfo()->SkillCooldownInfoMap;
			if (Separate_Map.Contains(ActiveSkillProxyPtr->GetID()))
			{
				auto CDSPtr = Separate_Map[ActiveSkillProxyPtr->GetID()];
				CDSPtr->SetCooldown(
					SkillCooldownInfoMap[ActiveSkillProxyPtr->Level]
				);
				CDSPtr->ResetCooldownTime();

				CD_FASI_Container.UpdateItem(CDSPtr);
			}
			else
			{
				TSharedPtr<FSkillCooldownHelper> SkillCooldownHelperSPtr = MakeShared<FSkillCooldownHelper>();

				SkillCooldownHelperSPtr->SkillProxy_ID = ActiveSkillProxyPtr->GetID();

				SkillCooldownHelperSPtr->SetCooldown(
					SkillCooldownInfoMap[ActiveSkillProxyPtr->Level]
				);
				SkillCooldownHelperSPtr->ResetCooldownTime();

				Separate_Map.Add(ActiveSkillProxyPtr->GetID(), SkillCooldownHelperSPtr);

				CD_FASI_Container.AddItem(SkillCooldownHelperSPtr);
			}
		}

		// 公共CD
		auto SkillCommonCooldownInfoMap = ActiveSkillProxyPtr->GetTableRowProxy_ActiveSkillExtendInfo()->SkillCommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			if (Common_Map.Contains(Iter))
			{
				auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
				if (CommonCooldownInfoPtr)
				{
					auto CDSPtr = Common_Map[Iter];
					CDSPtr->SetCooldown(CommonCooldownInfoPtr->CoolDownTime);
					CDSPtr->ResetCooldownTime();

					CD_FASI_Container.UpdateItem(CDSPtr);
				}
			}
			else
			{
				TSharedPtr<FSkillCooldownHelper> SkillCooldownHelperSPtr = MakeShared<FSkillCooldownHelper>();

				SkillCooldownHelperSPtr->SkillType = Iter;

				auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
				if (CommonCooldownInfoPtr)
				{
					SkillCooldownHelperSPtr->SetCooldown(CommonCooldownInfoPtr->CoolDownTime);
					SkillCooldownHelperSPtr->ResetCooldownTime();
				}

				Common_Map.Add(Iter, SkillCooldownHelperSPtr);

				CD_FASI_Container.AddItem(SkillCooldownHelperSPtr);
			}
		}
	}
#endif
}

TSharedPtr<FSkillCooldownHelper> UCDCaculatorComponent::GetCooldown(const FActiveSkillProxy* ActiveSkillProxyPtr) const
{
	TSharedPtr<FSkillCooldownHelper> MaxCD = nullptr;

	// 独立CD
	{
		auto SkillCooldownInfoMap = ActiveSkillProxyPtr->GetTableRowProxy_ActiveSkillExtendInfo()->SkillCooldownInfoMap;
		if (Separate_Map.Contains(ActiveSkillProxyPtr->GetID()))
		{
			return Separate_Map[ActiveSkillProxyPtr->GetID()];
		}
	}

	// 公共CD
	auto SkillCommonCooldownInfoMap = ActiveSkillProxyPtr->GetTableRowProxy_ActiveSkillExtendInfo()->SkillCommonCooldownInfoMap;
	for (const auto Iter : SkillCommonCooldownInfoMap)
	{
		if (Common_Map.Contains(Iter))
		{
			auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
			if (CommonCooldownInfoPtr)
			{
				return Common_Map[Iter];
			}
		}
	}

	return nullptr;
}

void UCDCaculatorComponent::ApplyCooldown(FConsumableProxy* ConsumableProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 独立CD
		{
			const auto CD = ConsumableProxySPtr->GetTableRowProxy_Consumable()->CD;
			if (Separate_Map.Contains(ConsumableProxySPtr->GetID()))
			{
				auto CDSPtr = Separate_Map[ConsumableProxySPtr->GetID()];
				CDSPtr->SetCooldown(CD);
				CDSPtr->ResetCooldownTime();

				CD_FASI_Container.UpdateItem(CDSPtr);
			}
			else
			{
				TSharedPtr<FSkillCooldownHelper> SkillCooldownHelperSPtr = MakeShared<FSkillCooldownHelper>();

				SkillCooldownHelperSPtr->SkillProxy_ID = ConsumableProxySPtr->GetID();

				SkillCooldownHelperSPtr->SetCooldown(CD);
				SkillCooldownHelperSPtr->ResetCooldownTime();

				Separate_Map.Add(ConsumableProxySPtr->GetID(), SkillCooldownHelperSPtr);

				CD_FASI_Container.AddItem(SkillCooldownHelperSPtr);
			}
		}

		// 公共CD
		auto SkillCommonCooldownInfoMap = ConsumableProxySPtr->GetTableRowProxy_Consumable()->CommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			if (Common_Map.Contains(Iter))
			{
				auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
				if (CommonCooldownInfoPtr)
				{
					auto CDSPtr = Common_Map[Iter];
					CDSPtr->SetCooldown(CommonCooldownInfoPtr->CoolDownTime);
					CDSPtr->ResetCooldownTime();

					CD_FASI_Container.UpdateItem(CDSPtr);
				}
			}
			else
			{
				TSharedPtr<FSkillCooldownHelper> SkillCooldownHelperSPtr = MakeShared<FSkillCooldownHelper>();

				SkillCooldownHelperSPtr->SkillType = Iter;

				auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
				if (CommonCooldownInfoPtr)
				{
					SkillCooldownHelperSPtr->SetCooldown(CommonCooldownInfoPtr->CoolDownTime);
					SkillCooldownHelperSPtr->ResetCooldownTime();
				}

				Common_Map.Add(Iter, SkillCooldownHelperSPtr);

				CD_FASI_Container.AddItem(SkillCooldownHelperSPtr);
			}
		}
	}
#endif
}

TSharedPtr<FSkillCooldownHelper> UCDCaculatorComponent::GetCooldown(const FConsumableProxy* ConsumableProxySPtr) const
{
	TSharedPtr<FSkillCooldownHelper> MaxCD = nullptr;

	float RemainingCooldown = 0.f;
	float RemainingCooldownPercent = 0.f;

	// 独立CD
	{
		if (Separate_Map.Contains(ConsumableProxySPtr->GetID()))
		{
			MaxCD = Separate_Map[ConsumableProxySPtr->GetID()];
			MaxCD->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
		}
	}

	// 公共CD
	auto SkillCommonCooldownInfoMap = ConsumableProxySPtr->GetTableRowProxy_Consumable()->CommonCooldownInfoMap;
	for (const auto Iter : SkillCommonCooldownInfoMap)
	{
		if (Common_Map.Contains(Iter))
		{
			auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
			if (CommonCooldownInfoPtr)
			{
				auto TempSPtr = Common_Map[Iter];

				float TempRemainingCooldown = 0.f;
				float TempRemainingCooldownPercent = 0.f;
				TempSPtr->GetRemainingCooldown(TempRemainingCooldown, TempRemainingCooldownPercent);

				if (TempRemainingCooldown > RemainingCooldown)
				{
					MaxCD = TempSPtr;
					RemainingCooldown = TempRemainingCooldown;
					RemainingCooldownPercent = TempRemainingCooldownPercent;
				}
			}
		}
	}

	return MaxCD;
}

void UCDCaculatorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		CD_FASI_Container.CDCaculatorComponentPtr = this;
	}
}
