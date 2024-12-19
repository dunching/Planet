
#include "TalentAllocationComponent.h"

#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "ItemProxy_Minimal.h"
#include "HoldingItemsComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "ProxyProcessComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "PlanetControllerInterface.h"
#include "BaseFeatureComponent.h"
#include "SceneProxyExtendInfo.h"
#include "TalentInfo.h"

FName UTalentAllocationComponent::ComponentName = TEXT("TalentAllocationComponent");

struct FPropertySettlementModify_Talent : public FPropertySettlementModify
{
	float Multiple = 1.f;
	FPropertySettlementModify_Talent(float InMultiple) :
		FPropertySettlementModify(10),
		Multiple(InMultiple)
	{

	}

	virtual int32 SettlementModify(const TMap<FGameplayTag, int32>& ValueMap)const override
	{
		const auto Result = FPropertySettlementModify::SettlementModify(ValueMap);
		return Result * Multiple;
	}
};

UTalentAllocationComponent::UTalentAllocationComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	
	SetIsReplicatedByDefault(true);
}

void UTalentAllocationComponent::AddCheck(const FTalentHelper& TalentHelper)
{
	auto Iter = TalentMap.Find(TalentHelper.IconSocket);
	if (Iter)
	{
		if (GetUsedTalentPointNum() < GetTotalTalentPointNum())
		{
			if (Iter->Level < Iter->TotalLevel)
			{
				auto OldValue = *Iter;
				Iter->Level++;

				CalculorUsedTalentPointNum();
#if UE_EDITOR || UE_SERVER
				if (GetNetMode() == NM_DedicatedServer)
				{
					Talent_FASI_Container.UpdateItem(*Iter);
				}
#endif
			}
		}
	}

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		AddCheck_Server(TalentHelper);
	}
#endif
}

void UTalentAllocationComponent::AddCheck_Server_Implementation(const FTalentHelper& TalentHelper)
{
	AddCheck(TalentHelper);
	UpdateTalent(TalentHelper);
}

void UTalentAllocationComponent::SubCheck(const FTalentHelper& TalentHelper)
{
	auto Iter = TalentMap.Find(TalentHelper.IconSocket);
	if (Iter)
	{
		if (Iter->Level > 0)
		{
			auto OldValue = *Iter;
			Iter->Level--;

			CalculorUsedTalentPointNum();
		}
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			Talent_FASI_Container.UpdateItem(*Iter);
		}
#endif
	}

#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		SubCheck_Server(TalentHelper);
	}
#endif
}

void UTalentAllocationComponent::SubCheck_Server_Implementation(const FTalentHelper& TalentHelper)
{
	SubCheck(TalentHelper);
	UpdateTalent(TalentHelper);
}

void UTalentAllocationComponent::UpdateTalent(const FTalentHelper& TalentHelper)
{
	auto TalentHelper_Iter = TalentMap.Find(TalentHelper.IconSocket);
	if (!TalentHelper_Iter)
	{
		return;
	}
	switch (TalentHelper_Iter->PointType)
	{
	case EPointType::kSkill:
	{
		if (std::get<EPointSkillType>(TalentHelper_Iter->Type) == PreviousSkillType)
		{
		}
		else
		{
			auto CharacterPtr = GetOwner<FOwnerType>();
			if (CharacterPtr)
			{
			}
		}
	}
	break;
	case EPointType::kProperty:
	{
		auto CharacterPtr = GetOwner<FOwnerType>();
		if (CharacterPtr)
		{
			const auto DataSource = UGameplayTagsLibrary::DataSource_TalentModify;

			const auto PointPropertyType = std::get<EPointPropertyType>(TalentHelper_Iter->Type);
			switch (PointPropertyType)
			{
			case EPointPropertyType::kLiDao:
			{
				auto PAD_Talent_PropertyPtr = USceneProxyExtendInfoMap::GetInstance()->GetTalent_Property(PointPropertyType);

				if (TalentHelper_Iter->Level > 0)
				{
					// 增加的数据
					auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

					GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

					FGAEventData GAEventData(
						CharacterPtr,
						CharacterPtr
					);

					GAEventData.DataSource = DataSource;
					GAEventData.DataModify = {
						{ECharacterPropertyType::LiDao,TalentHelper_Iter->Level }
					};
					for (const auto& Iter : PAD_Talent_PropertyPtr->AddValueMap)
					{
						GAEventData.DataModify.Add(Iter.Key, TalentHelper_Iter->Level * Iter.Value);
					}
					GAEventData.bIsOverlapData = true;

					GAEventDataPtr->DataAry.Add(GAEventData);

					CharacterPtr->GetBaseFeatureComponent()->SendEventImp(GAEventDataPtr);

					// 数据修正
					for (const auto& Iter : PAD_Talent_PropertyPtr->ModifyValueMap)
					{
						const auto MultipleValue = 1 + (TalentHelper_Iter->Level * Iter.Value);

						// if (ModifyMap.Contains(PointPropertyType) && ModifyMap[PointPropertyType].IsValid())
						// {
						// 	ModifyMap[PointPropertyType].Pin()->Multiple = MultipleValue;
						// 	CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.UpdateSettlementModify(
						// 		ModifyMap[PointPropertyType].Pin()
						// 	);
						// }
						// else
						// {
						// 	auto SPtr = MakeShared<FPropertySettlementModify_Talent>(MultipleValue);
						// 	ModifyMap.Add(PointPropertyType, SPtr);
						// 	CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddSettlementModify(SPtr);
						// }
					}
				}
				else
				{
					CharacterPtr->GetBaseFeatureComponent()->ClearData2Self(GetAllData(), DataSource);
					for (const auto& Iter : PAD_Talent_PropertyPtr->ModifyValueMap)
					{
						if (ModifyMap.Contains(PointPropertyType) && ModifyMap[PointPropertyType].IsValid())
						{
							// CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.RemoveSettlementModify(
							// 	ModifyMap[PointPropertyType].Pin()
							// );
						}
					}
				}
			}
			break;
			case EPointPropertyType::kGenGu:
			{
			}
			break;
			case EPointPropertyType::kShenFa:
			{
			}
			break;
			case EPointPropertyType::kDongCha:
			{
			}
			break;
			case EPointPropertyType::kTianZi:
			{
			}
			break;
			}
		}
	}
	break;
	}
}

void UTalentAllocationComponent::Clear(FTalentHelper& TalentHelper)
{
	auto Iter = TalentMap.Find(TalentHelper.IconSocket);
	if (Iter)
	{
		Iter->Level = 0;
		Talent_FASI_Container.UpdateItem(*Iter);
	}
}

FTalentHelper UTalentAllocationComponent::GetCheck(const FTalentHelper& TalentHelper)
{
	if (auto Iter = TalentMap.Find(TalentHelper.IconSocket))
	{
		return *Iter;
	}
	return FTalentHelper();
}

int32 UTalentAllocationComponent::GetTotalTalentPointNum() const
{
	return TotalTalentPointNum;
}

int32 UTalentAllocationComponent::GetUsedTalentPointNum() const
{
	return UsedTalentPointNum;
}

void UTalentAllocationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Talent_FASI_Container);
}

void UTalentAllocationComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		Talent_FASI_Container.TalentAllocationComponentPtr = this;
	}
}

void UTalentAllocationComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		InitialTalentData();
	}
#endif

	CalculorUsedTalentPointNum();
}

void UTalentAllocationComponent::InitialTalentData()
{
	struct FSkillHelper
	{
		FName Text;
		EPointSkillType PointSkillType;
	};
	TArray<FSkillHelper>SkillAry
	{
		{TEXT("Talent.NuQi"), EPointSkillType::kNuQi },
		{TEXT("Talent.YinYang"), EPointSkillType::kYinYang },
		{TEXT("Talent.DuXing"), EPointSkillType::kDuXing },
		{TEXT("Talent.FaLi"), EPointSkillType::kFaLi },
		{TEXT("Talent.GongMing"), EPointSkillType::kGongMing },
	};
	for (auto Iter : SkillAry)
	{
		FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(Iter.Text);

		FTalentHelper TalentHelper;

		TalentHelper.IconSocket = GameplayTag;
		TalentHelper.PointType = EPointType::kSkill;
		TalentHelper.Type = Iter.PointSkillType;
		TalentHelper.Level = 0;
		TalentHelper.TotalLevel = 3;

		TalentMap.Add(GameplayTag, TalentHelper);
		Talent_FASI_Container.AddItem(TalentHelper);
	}

	struct FPropertyHelper
	{
		FName Text;
		EPointPropertyType PointPropertyType;
	};
	TArray<FPropertyHelper>PropertyAry
	{
		{TEXT("Talent.LiDao.1"), EPointPropertyType::kLiDao },
		{TEXT("Talent.GenGu.1"), EPointPropertyType::kGenGu },
		{TEXT("Talent.ShenFa.1"), EPointPropertyType::kShenFa },
		{TEXT("Talent.DongCha.1"), EPointPropertyType::kDongCha },
		{TEXT("Talent.TianZi.1"), EPointPropertyType::kTianZi },

		{TEXT("Talent.LiDao.2"), EPointPropertyType::kLiDao },
		{TEXT("Talent.GenGu.2"), EPointPropertyType::kGenGu },
		{TEXT("Talent.ShenFa.2"), EPointPropertyType::kShenFa },
		{TEXT("Talent.DongCha.2"), EPointPropertyType::kDongCha },
		{TEXT("Talent.TianZi.2"), EPointPropertyType::kTianZi },
	};
	for (auto Iter : PropertyAry)
	{
		FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(Iter.Text);

		FTalentHelper TalentHelper;

		TalentHelper.IconSocket = GameplayTag;
		TalentHelper.PointType = EPointType::kProperty;
		TalentHelper.Type = Iter.PointPropertyType;
		TalentHelper.Level = 0;
		TalentHelper.TotalLevel = 3;

		TalentMap.Add(GameplayTag, TalentHelper);
		Talent_FASI_Container.AddItem(TalentHelper);
	}
}

void UTalentAllocationComponent::SyncToHolding()
{
}

void UTalentAllocationComponent::UpdateTalent_Client(const FTalentHelper& TalentHelper)
{
	TalentMap.Add(TalentHelper.IconSocket, TalentHelper);
}

void UTalentAllocationComponent::CalculorUsedTalentPointNum()
{
	const auto OldValue = UsedTalentPointNum;

	UsedTalentPointNum = 0;
	for (const auto& Iter : TalentMap)
	{
		UsedTalentPointNum += Iter.Value.Level;
	}

	CallbackContainerHelper.ValueChanged(OldValue, UsedTalentPointNum);
}

