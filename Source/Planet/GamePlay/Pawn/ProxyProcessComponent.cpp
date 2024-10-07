
#include "ProxyProcessComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"


#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "PlanetPlayerState.h"
#include "BasicFuturesBase.h"
#include "GAEvent_Send.h"
#include "GAEvent_Received.h"
#include "SceneElement.h"
#include "Weapon_Base.h"
#include "Skill_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "AssetRefMap.h"
#include "Skill_Talent_NuQi.h"
#include "Skill_Talent_YinYang.h"
#include "Skill_Element_Gold.h"
#include "InputProcessorSubSystem.h"
#include "Tool_PickAxe.h"
#include "HumanRegularProcessor.h"
#include "HumanCharacter.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitTable.h"
#include "Skill_Active_Control.h"
#include "BaseFeatureComponent.h"
#include "Skill_WeaponActive_Base.h"
#include "HoldingItemsComponent.h"

FName UProxyProcessComponent::ComponentName = TEXT("ProxyProcessComponent");

UProxyProcessComponent::UProxyProcessComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UProxyProcessComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		TArray<FGameplayTag>Ary
		{
			UGameplayTagsSubSystem::GetInstance()->ActiveSocket1,
			UGameplayTagsSubSystem::GetInstance()->ActiveSocket2,
			UGameplayTagsSubSystem::GetInstance()->ActiveSocket3,
			UGameplayTagsSubSystem::GetInstance()->ActiveSocket4,

			UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1,
			UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2,

			UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket_1,
			UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket_2,

			UGameplayTagsSubSystem::GetInstance()->ConsumableSocket1,
			UGameplayTagsSubSystem::GetInstance()->ConsumableSocket2,
			UGameplayTagsSubSystem::GetInstance()->ConsumableSocket3,
			UGameplayTagsSubSystem::GetInstance()->ConsumableSocket4,

			UGameplayTagsSubSystem::GetInstance()->PassiveSocket_1,
			UGameplayTagsSubSystem::GetInstance()->PassiveSocket_2,
			UGameplayTagsSubSystem::GetInstance()->PassiveSocket_3,
			UGameplayTagsSubSystem::GetInstance()->PassiveSocket_4,
			UGameplayTagsSubSystem::GetInstance()->PassiveSocket_5,
		};

		for (const auto& Iter : Ary)
		{
			auto SocketSPtr = MakeShared<FSocket_FASI>();
			SocketSPtr->Socket = Iter;

			SocketMap.Add(SocketSPtr->Socket, SocketSPtr);
			AllocationSkills_Container.AddItem(SocketSPtr);
		}
	}
#endif
}

void UProxyProcessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UProxyProcessComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UProxyProcessComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AllocationSkills_Container, Params);

	Params.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentWeaponSocket, Params);
}

void UProxyProcessComponent::UpdateSocket(
	const TSharedPtr<FSocket_FASI>& SkillsSocketInfo
)
{
	if (SkillsSocketInfo->ProxySPtr)
	{
		SkillsSocketInfo->ProxyID = SkillsSocketInfo->ProxySPtr->GetID();
	}
	else
	{
		SkillsSocketInfo->ProxyID = FGuid();
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		UpdateSocket_Server(*SkillsSocketInfo);
	}
#endif

	Update(SkillsSocketInfo);
}

void UProxyProcessComponent::UpdateSocket_Server_Implementation(const FSocket_FASI& Socket)
{
	auto SkillsSocketInfo = MakeShared<FSocket_FASI>();
	*SkillsSocketInfo = Socket;

	SkillsSocketInfo->ProxySPtr = AllocationSkills_Container.HoldingItemsComponentPtr->FindProxy(Socket.ProxyID);

	Update(SkillsSocketInfo);
// 	if (SocketMap.Contains(Socket.Socket))
// 	{
// 		SocketMap[Socket.Socket]->ProxySPtr = AllocationSkills_Container.HoldingItemsComponentPtr->FindProxy(Socket.ProxyID);
// 
// 		Update(SocketMap[Socket.Socket]);
// 	}
}

void UProxyProcessComponent::ActiveWeapon_Server_Implementation()
{
	ActiveWeapon();
}

void UProxyProcessComponent::SwitchWeapon_Server_Implementation()
{
	SwitchWeapon();
}

void UProxyProcessComponent::RetractputWeapon_Server_Implementation()
{
	RetractputWeapon();
}

void UProxyProcessComponent::ActiveAction_Server_Implementation(
	const FSocket_FASI& Socket,
	bool bIsAutomaticStop
)
{
	Active(Socket.Socket);
}

void UProxyProcessComponent::CancelAction_Server_Implementation(const FSocket_FASI& Socket)
{
	Cancel(Socket.Socket);
}

void UProxyProcessComponent::ActiveWeapon()
{
	const auto WeaponsMap = SocketMap;

	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1];
		if (WeaponSocketSPtr->ProxyID.IsValid())
		{
			SwitchWeaponImp(WeaponSocketSPtr->Socket);
		}
	}
	else if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2];
		if (WeaponSocketSPtr->ProxyID.IsValid())
		{
			SwitchWeaponImp(WeaponSocketSPtr->Socket);
		}
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		ActiveWeapon_Server();
	}
#endif

}

void UProxyProcessComponent::SwitchWeapon()
{
	const auto WeaponsMap = SocketMap;
	if (
		(WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1)) ||
		(WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2))
		)
	{
		if (UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1 == CurrentWeaponSocket)
		{
			SwitchWeaponImp(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2);
		}
		else
		{
			SwitchWeaponImp(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1);
		}
	}
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		SwitchWeapon_Server();
	}
#endif
}

void UProxyProcessComponent::RetractputWeapon()
{
	SwitchWeaponImp(FGameplayTag::EmptyTag);
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		RetractputWeapon_Server();
	}
#endif
}

int32 UProxyProcessComponent::GetCurrentWeaponAttackDistance() const
{
	auto SPtr = GetActivedWeapon();
	if (SPtr)
	{
		return SPtr->GetMaxAttackDistance();
	}

	return 100;
}

void UProxyProcessComponent::GetWeapon(
	TSharedPtr<FSocket_FASI>& FirstWeaponSocketInfoSPtr,
	TSharedPtr<FSocket_FASI>& SecondWeaponSocketInfoSPtr
)
{
	const auto WeaponsMap = SocketMap;
	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1))
	{
		FirstWeaponSocketInfoSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1];
	}
	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2))
	{
		SecondWeaponSocketInfoSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2];
	}
}

void UProxyProcessComponent::GetWeaponSkills(
	TSharedPtr<FWeaponSkillProxy>& FirstWeaponSkillSPtr,
	TSharedPtr<FWeaponSkillProxy>& SecondWeaponSkillSPtr
)
{
	TSharedPtr<FSocket_FASI> FirstWeaponSocketInfoSPtr;
	TSharedPtr<FSocket_FASI> SecondWeaponSocketInfoSPtr;
	GetWeapon(FirstWeaponSocketInfoSPtr, SecondWeaponSocketInfoSPtr);
	if (FirstWeaponSocketInfoSPtr && FirstWeaponSocketInfoSPtr->ProxySPtr)
	{
		auto WeaponSPtr = DynamicCastSharedPtr<FWeaponProxy>(FirstWeaponSocketInfoSPtr->ProxySPtr);
		FirstWeaponSkillSPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(WeaponSPtr->GetWeaponSkill());
	}
	if (SecondWeaponSocketInfoSPtr && SecondWeaponSocketInfoSPtr->ProxySPtr)
	{
		auto WeaponSPtr = DynamicCastSharedPtr<FWeaponProxy>(SecondWeaponSocketInfoSPtr->ProxySPtr);
		SecondWeaponSkillSPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(WeaponSPtr->GetWeaponSkill());
	}
}

TSharedPtr<FWeaponProxy> UProxyProcessComponent::GetActivedWeapon() const
{
	return FindWeaponSocket(CurrentWeaponSocket);
}

TSharedPtr<FWeaponProxy> UProxyProcessComponent::FindWeaponSocket(const FGameplayTag& Tag) const
{
	if (SocketMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FWeaponProxy>(SocketMap[Tag]->ProxySPtr);
	}

	return nullptr;
}

TSharedPtr<FWeaponSkillProxy> UProxyProcessComponent::FindWeaponSkillSocket(const FGameplayTag& Tag) const
{
	if (SocketMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FWeaponSkillProxy>(SocketMap[Tag]->ProxySPtr);
	}

	return nullptr;
}

bool UProxyProcessComponent::ActiveAction(
	const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	auto WeaponSPtr = FindSocket(CurrentWeaponSocket);
	if (WeaponSPtr->Socket == CanbeActivedInfoSPtr->Socket)
	{
		ActiveAction_Server(*WeaponSPtr, bIsAutomaticStop);
	}
	else
	{
		ActiveAction_Server(*CanbeActivedInfoSPtr, bIsAutomaticStop);
	}

	return true;
}

void UProxyProcessComponent::CancelAction(const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr)
{
	auto WeaponSPtr = FindSocket(CurrentWeaponSocket);
	if (WeaponSPtr->Key == CanbeActivedInfoSPtr->Key)
	{
		CancelAction_Server(*WeaponSPtr);
	}
	else
	{
		CancelAction_Server(*CanbeActivedInfoSPtr);
	}
}

TArray<TSharedPtr<FSocket_FASI>> UProxyProcessComponent::GetCanbeActiveAction() const
{
	TArray<TSharedPtr<FSocket_FASI>>Result;

	for (auto Iter : SocketMap)
	{
		if (Iter.Value->Socket.MatchesTag(UGameplayTagsSubSystem::GetInstance()->ConsumableSocket))
		{
			Result.Add(Iter.Value);
		}
		if (Iter.Value->Socket.MatchesTag(UGameplayTagsSubSystem::GetInstance()->ActiveSocket))
		{
			Result.Add(Iter.Value);
		}
	}

	if (SocketMap.Contains(CurrentWeaponSocket))
	{
		Result.Add(SocketMap[CurrentWeaponSocket]);
	}

	return Result;
}

bool UProxyProcessComponent::ActivedCorrespondingWeapon(const FSocket_FASI& Socket)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		ActivedCorrespondingWeapon_Server(Socket);
	}
#endif

	if (Socket.ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		auto ActiveSkillUnitPtr = DynamicCastSharedPtr<FActiveSkillProxy>(Socket.ProxySPtr);
		const auto RequireWeaponUnitType = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->RequireWeaponUnitType;

		const auto WeaponsMap = SocketMap;

		if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1))
		{
			auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponSocket_1];

			if (WeaponSocketSPtr->ProxySPtr->GetUnitType() == RequireWeaponUnitType)
			{
				SwitchWeaponImp(WeaponSocketSPtr->Socket);

				return true;
			}
		}
		else if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2))
		{
			auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponSocket_2];

			if (WeaponSocketSPtr->ProxySPtr->GetUnitType() == RequireWeaponUnitType)
			{
				SwitchWeaponImp(WeaponSocketSPtr->Socket);

				return true;
			}
		}
	}

	return false;
}

void UProxyProcessComponent::ActivedCorrespondingWeapon_Server_Implementation(const FSocket_FASI& Socket)
{
	ActivedCorrespondingWeapon(Socket);
}

void UProxyProcessComponent::OnRep_AllocationChanged()
{
}

void UProxyProcessComponent::OnRep_CurrentActivedSocketChanged()
{
}

void UProxyProcessComponent::SwitchWeaponImp(const FGameplayTag& NewWeaponSocket)
{
	if (NewWeaponSocket == CurrentWeaponSocket)
	{
	}
	else
	{
		auto PreviousWeaponSocketSPtr = FindWeaponSocket(CurrentWeaponSocket);
		if (PreviousWeaponSocketSPtr)
		{
#if UE_EDITOR || UE_SERVER
			if (GetNetMode() == NM_DedicatedServer)
			{
				PreviousWeaponSocketSPtr->RetractputWeapon();
				PreviousWeaponSocketSPtr->Update2Client();
			}
#endif
		}

		auto NewWeaponSocketSPtr = FindWeaponSocket(NewWeaponSocket);
		if (NewWeaponSocketSPtr)
		{
#if UE_EDITOR || UE_SERVER
			if (GetNetMode() == NM_DedicatedServer)
			{
				NewWeaponSocketSPtr->ActiveWeapon();
				NewWeaponSocketSPtr->Update2Client();
			}
#endif
		}

		CurrentWeaponSocket = NewWeaponSocket;

		OnCurrentWeaponChanged();
	}
}

TMap<FGameplayTag, TSharedPtr<FSocket_FASI>> UProxyProcessComponent::GetAllSocket() const
{
	return SocketMap;
}

TSharedPtr<FSocket_FASI> UProxyProcessComponent::FindSocket(const FGameplayTag& Tag) const
{
	if (SocketMap.Contains(Tag))
	{
		return SocketMap[Tag];
	}

	return nullptr;
}

TSharedPtr<FActiveSkillProxy> UProxyProcessComponent::FindActiveSkillSocket(const FGameplayTag& Tag)const
{
	if (SocketMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FActiveSkillProxy>(SocketMap[Tag]->ProxySPtr);
	}

	return nullptr;
}

void UProxyProcessComponent::Add(const TSharedPtr<FSocket_FASI>& Socket)
{
	SocketMap.Add(Socket->Socket, Socket);
	if (Socket->ProxySPtr)
	{
		Socket->ProxySPtr->Allocation();

#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			Socket->ProxySPtr->Update2Client();
		}
#endif
	}
}

void UProxyProcessComponent::Update(const TSharedPtr<FSocket_FASI>& Socket)
{
	if (SocketMap.Contains(Socket->Socket))
	{
		if (SocketMap[Socket->Socket]->ProxySPtr == Socket->ProxySPtr)
		{
		}
		else
		{
			if (SocketMap[Socket->Socket]->ProxySPtr)
			{
				SocketMap[Socket->Socket]->ProxySPtr->UnAllocation();
#if UE_EDITOR || UE_SERVER
				if (GetNetMode() == NM_DedicatedServer)
				{
					SocketMap[Socket->Socket]->ProxySPtr->Update2Client();
				}
#endif
			}

			if (Socket->ProxySPtr)
			{
				Socket->ProxySPtr->Allocation();
#if UE_EDITOR || UE_SERVER
				if (GetNetMode() == NM_DedicatedServer)
				{
					Socket->ProxySPtr->Update2Client();
				}
#endif
			}

			SocketMap[Socket->Socket]->ProxyID = Socket->ProxyID;
			SocketMap[Socket->Socket]->ProxySPtr = Socket->ProxySPtr;

#if UE_EDITOR || UE_SERVER
			if (GetNetMode() == NM_DedicatedServer)
			{
				AllocationSkills_Container.UpdateItem(Socket);
			}
#endif
		}
	}
}

bool UProxyProcessComponent::Active(const TSharedPtr<FSocket_FASI>& Socket)
{
	return Active(Socket->Socket);
}

bool UProxyProcessComponent::Active(const FGameplayTag& Socket)
{
	if (SocketMap.Contains(Socket))
	{
		if (SocketMap[Socket]->ProxySPtr)
		{
			return SocketMap[Socket]->ProxySPtr->Active();
		}
	}

	return false;
}

void UProxyProcessComponent::Cancel(const TSharedPtr<FSocket_FASI>& Socket)
{
	Cancel(Socket->Socket);
}

void UProxyProcessComponent::Cancel(const FGameplayTag& Socket)
{
	if (SocketMap.Contains(Socket))
	{
		if (SocketMap[Socket]->ProxySPtr)
		{
			SocketMap[Socket]->ProxySPtr->Cancel();
		}
	}
}
