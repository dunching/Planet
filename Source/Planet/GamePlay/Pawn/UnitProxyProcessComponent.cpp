
#include "UnitProxyProcessComponent.h"

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
#include "BaseFeatureGAComponent.h"
#include "Skill_WeaponActive_Base.h"
#include "HoldingItemsComponent.h"

FName UUnitProxyProcessComponent::ComponentName = TEXT("InteractiveSkillComponent");

UUnitProxyProcessComponent::UUnitProxyProcessComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	AllocationSkills_Container.UnitProxyProcessComponentPtr = this;
}

void UUnitProxyProcessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UUnitProxyProcessComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UUnitProxyProcessComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_SkipReplay;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AllocationSkills_Container, Params);
}

void UUnitProxyProcessComponent::RegisterWeapon(
	const TSharedPtr<FSocket_FASI>& WeaponSocket
)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		RegisterWeapon_Server(*WeaponSocket);
	}
#endif

	Update(WeaponSocket);
}

void UUnitProxyProcessComponent::RegisterMultiGAs(
	const TSharedPtr<FSocket_FASI>& SkillSocket
)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		RegisterMultiGAs_Server(*SkillSocket);
	}
#endif

	Update(SkillSocket);
}

void UUnitProxyProcessComponent::RegisterMultiGAs_Server_Implementation(const FSocket_FASI& SkillSocket)
{
	auto SkillsSocketInfo = MakeShared<FSocket_FASI>();
	*SkillsSocketInfo = SkillSocket;

	Update(SkillsSocketInfo);
}

void UUnitProxyProcessComponent::RegisterWeapon_Server_Implementation(const FSocket_FASI& Socket)
{
	auto SkillsSocketInfo = MakeShared<FSocket_FASI>();
	*SkillsSocketInfo = Socket;

	Update(SkillsSocketInfo);
}

void UUnitProxyProcessComponent::ActiveAction_Server_Implementation(
	const FSocket_FASI& Socket,
	bool bIsAutomaticStop
)
{
	Active(Socket.Socket);
}

void UUnitProxyProcessComponent::CancelAction_Server_Implementation(const FSocket_FASI& Socket)
{
	Cancel(Socket.Socket);
}

void UUnitProxyProcessComponent::ActiveWeapon()
{
	const auto WeaponsMap = SkillsMap;

	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1];
		SwitchWeaponImp(WeaponSocketSPtr->Socket);
	}
	else if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2];
		SwitchWeaponImp(WeaponSocketSPtr->Socket);
	}
}

void UUnitProxyProcessComponent::SwitchWeapon()
{
	const auto WeaponsMap = SkillsMap;
	if (
		(WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1)) ||
		(WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
		)
	{
		if (UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1 == PreviousWeaponSocket)
		{
			SwitchWeaponImp(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2);
		}
		else
		{
			SwitchWeaponImp(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1);
		}
	}
}

void UUnitProxyProcessComponent::RetractputWeapon()
{
	SwitchWeaponImp(FGameplayTag::EmptyTag);
}

int32 UUnitProxyProcessComponent::GetCurrentWeaponAttackDistance() const
{
	return 100;
}

void UUnitProxyProcessComponent::GetWeapon(
	TSharedPtr<FSocket_FASI>& FirstWeaponSocketInfoSPtr,
	TSharedPtr<FSocket_FASI>& SecondWeaponSocketInfoSPtr
)
{
	const auto WeaponsMap = SkillsMap;
	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1))
	{
		FirstWeaponSocketInfoSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1];
	}
	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
	{
		SecondWeaponSocketInfoSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2];
	}
}

TSharedPtr<FWeaponProxy> UUnitProxyProcessComponent::GetActivedWeapon() const
{
	return FindWeaponSocket(PreviousWeaponSocket);
}

TSharedPtr<FWeaponProxy> UUnitProxyProcessComponent::FindWeaponSocket(const FGameplayTag& Tag) const
{
	if (SkillsMap.Contains(Tag))
	{
		return DynamicCastSharedPtr<FWeaponProxy>(SkillsMap[Tag]->UnitPtr.Pin());
	}

	return nullptr;
}

bool UUnitProxyProcessComponent::ActiveAction(
	const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	ActiveAction_Server(*CanbeActivedInfoSPtr, bIsAutomaticStop);

	return true;
}

void UUnitProxyProcessComponent::CancelAction(const TSharedPtr<FSocket_FASI>& CanbeActivedInfoSPtr)
{
	CancelAction_Server(*CanbeActivedInfoSPtr);
}

TArray<TSharedPtr<FSocket_FASI>> UUnitProxyProcessComponent::GetCanbeActiveAction() const
{
	TArray<TSharedPtr<FSocket_FASI>>Result;

	for (auto Iter : SkillsMap)
	{
		Result.Add(Iter.Value);
	}

	return Result;
}

bool UUnitProxyProcessComponent::ActivedCorrespondingWeapon(const TSharedPtr<FActiveSkillProxy>& ActiveSkillUnitPtr)
{
	const auto RequireWeaponUnitType = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->RequireWeaponUnitType;

	const auto WeaponsMap = SkillsMap;

	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1];

		if (WeaponSocketSPtr->UnitPtr.Pin()->GetUnitType() == RequireWeaponUnitType)
		{
			SwitchWeaponImp(WeaponSocketSPtr->Socket);

			return true;
		}
	}
	else if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2];

		if (WeaponSocketSPtr->UnitPtr.Pin()->GetUnitType() == RequireWeaponUnitType)
		{
			SwitchWeaponImp(WeaponSocketSPtr->Socket);

			return true;
		}
	}

	return false;
}

void UUnitProxyProcessComponent::OnRep_ProxyChanged()
{

}

void UUnitProxyProcessComponent::SwitchWeaponImp_Implementation(const FGameplayTag& NewWeaponSocket)
{
	if (NewWeaponSocket == PreviousWeaponSocket)
	{
	}
	else
	{
		auto PreviousWeaponSocketSPtr = FindWeaponSocket(PreviousWeaponSocket);
		if (PreviousWeaponSocketSPtr)
		{
			PreviousWeaponSocketSPtr->RetractputWeapon();
		}

		auto NewWeaponSocketSPtr = FindWeaponSocket(NewWeaponSocket);
		if (NewWeaponSocketSPtr)
		{
			NewWeaponSocketSPtr->ActiveWeapon();
		}
		PreviousWeaponSocketSPtr = NewWeaponSocketSPtr;

		OnActivedWeaponChangedContainer(NewWeaponSocketSPtr);
	}
}

TMap<FGameplayTag, TSharedPtr<FSocket_FASI>> UUnitProxyProcessComponent::GetSkills() const
{
	return SkillsMap;
}

TSharedPtr<FSocket_FASI> UUnitProxyProcessComponent::FindSocket(const FGameplayTag& Tag) const
{
	if (SkillsMap.Contains(Tag))
	{
		return SkillsMap[Tag];
	}

	return nullptr;
}

TSharedPtr<FActiveSkillProxy> UUnitProxyProcessComponent::FindActiveSkillSocket(const FGameplayTag& Tag)const
{
	if (SkillsMap.Contains(Tag))
	{ 
		return DynamicCastSharedPtr<FActiveSkillProxy>(SkillsMap[Tag]->UnitPtr.Pin());
	}

	return nullptr;
}

void UUnitProxyProcessComponent::Update(const TSharedPtr<FSocket_FASI>& Socket)
{
}

bool UUnitProxyProcessComponent::Active(const TSharedPtr<FSocket_FASI>& Socket)
{
	return Active(Socket->Socket);
}

bool UUnitProxyProcessComponent::Active(const FGameplayTag& Socket)
{
	if (SkillsMap.Contains(Socket))
	{
		return SkillsMap[Socket]->UnitPtr.Pin()->Active();
	}

	return false;
}

void UUnitProxyProcessComponent::Cancel(const TSharedPtr<FSocket_FASI>& Socket)
{
	Cancel(Socket->Socket);
}

void UUnitProxyProcessComponent::Cancel(const FGameplayTag& Socket)
{
	if (SkillsMap.Contains(Socket))
	{
		SkillsMap[Socket]->UnitPtr.Pin()->Cancel();
	}
}
