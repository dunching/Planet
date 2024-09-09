
#include "UnitProxyProcessComponent.h"

#include <queue>
#include <map>

#include "GameplayAbilitySpec.h"

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
	AllocationSkills_Member = MakeShared<FAllocationSkills>();
}

void UUnitProxyProcessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UUnitProxyProcessComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UUnitProxyProcessComponent::RegisterWeapon(
	const TSharedPtr<FWeaponSocket>& WeaponSocket
)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		RegisterWeapon_Server(*WeaponSocket);
	}
#endif

	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Update(WeaponSocket);
}

void UUnitProxyProcessComponent::RegisterMultiGAs(
	const TSharedPtr<FSkillSocket>& SkillSocket
)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		RegisterMultiGAs_Server(*SkillSocket);
	}
#endif

	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Update(SkillSocket);
}

void UUnitProxyProcessComponent::RegisterMultiGAs_Server_Implementation(const FSkillSocket& SkillSocket)
{
	auto SkillsSocketInfo = MakeShared<FSkillSocket>();
	*SkillsSocketInfo = SkillSocket;

	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Update(SkillsSocketInfo);
}

void UUnitProxyProcessComponent::RegisterWeapon_Server_Implementation(const FWeaponSocket& Socket)
{
	auto SkillsSocketInfo = MakeShared<FWeaponSocket>();
	*SkillsSocketInfo = Socket;

	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Update(SkillsSocketInfo);
}

void UUnitProxyProcessComponent::ActiveAction_Server_Implementation(
	const FSocketBase& Socket,
	bool bIsAutomaticStop
)
{
	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Active(Socket.Socket);
}

void UUnitProxyProcessComponent::CancelAction_Server_Implementation(const FSocketBase& Socket)
{
	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Cancel(Socket.Socket);
}

void UUnitProxyProcessComponent::ActiveWeapon()
{
	auto AllocationSkills = GetAllocationSkills();

	auto WeaponsMap = AllocationSkills->GetWeaponsMap();

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
	auto AllocationSkills = GetAllocationSkills();

	auto WeaponsMap = AllocationSkills->GetWeaponsMap();
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
	TSharedPtr<FWeaponSocket>& FirstWeaponSocketInfoSPtr,
	TSharedPtr<FWeaponSocket>& SecondWeaponSocketInfoSPtr
)
{
	auto AllocationSkills = GetAllocationSkills();

	auto WeaponsMap = AllocationSkills->GetWeaponsMap();
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
	auto PreviousWeaponSocketSPtr = AllocationSkills_Member->FindWeapon(PreviousWeaponSocket);
	if (PreviousWeaponSocketSPtr)
	{
		return PreviousWeaponSocketSPtr->UnitPtr.Pin();
	}

	return nullptr;
}

TSharedPtr<FAllocationSkills> UUnitProxyProcessComponent::GetAllocationSkills() const
{
	return AllocationSkills_Member;
}

bool UUnitProxyProcessComponent::ActiveAction(
	const TSharedPtr<FSocketBase>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	ActiveAction_Server(*CanbeActivedInfoSPtr, bIsAutomaticStop);

	return true;
}

void UUnitProxyProcessComponent::CancelAction(const TSharedPtr<FSocketBase>& CanbeActivedInfoSPtr)
{
	CancelAction_Server(*CanbeActivedInfoSPtr);
}

TArray<TSharedPtr<FSocketBase>> UUnitProxyProcessComponent::GetCanbeActiveAction() const
{
	TArray<TSharedPtr<FSocketBase>>Result;

	auto AllocationSkills = GetAllocationSkills();

	auto SkillsMap = AllocationSkills->GetSkillsMap();
	auto WeaponsMap = AllocationSkills->GetWeaponsMap();

	for (auto Iter : SkillsMap)
	{
		Result.Add(Iter.Value);
	}

	for (auto Iter : WeaponsMap)
	{
		Result.Add(Iter.Value);
	}

	return Result;
}

bool UUnitProxyProcessComponent::ActivedCorrespondingWeapon(const TSharedPtr<FActiveSkillProxy>& ActiveSkillUnitPtr)
{
	const auto RequireWeaponUnitType = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->RequireWeaponUnitType;

	auto AllocationSkills = GetAllocationSkills();

	auto WeaponsMap = AllocationSkills->GetWeaponsMap();

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

void UUnitProxyProcessComponent::SwitchWeaponImp_Implementation(const FGameplayTag& NewWeaponSocket)
{
	if (NewWeaponSocket == PreviousWeaponSocket)
	{
	}
	else
	{
		auto PreviousWeaponSocketSPtr = AllocationSkills_Member->FindWeapon(PreviousWeaponSocket);
		if (PreviousWeaponSocketSPtr)
		{
			PreviousWeaponSocketSPtr->UnitPtr.Pin()->RetractputWeapon();
		}

		auto NewWeaponSocketSPtr = AllocationSkills_Member->FindWeapon(NewWeaponSocket);
		if (NewWeaponSocketSPtr)
		{
			NewWeaponSocketSPtr->UnitPtr.Pin()->ActiveWeapon();
		}
		PreviousWeaponSocketSPtr = NewWeaponSocketSPtr;

		OnActivedWeaponChangedContainer(NewWeaponSocketSPtr);
	}
}

TSharedPtr<FSkillSocket>UUnitProxyProcessComponent::FindSkill(const FGameplayTag& Tag)
{
	auto AllocationSkills = GetAllocationSkills();

	return AllocationSkills->FindSkill(Tag);
}

TMap<FGameplayTag, TSharedPtr<FSkillSocket>> UUnitProxyProcessComponent::GetSkills() const
{
	auto AllocationSkills = GetAllocationSkills();

	return AllocationSkills->GetSkillsMap();
}
