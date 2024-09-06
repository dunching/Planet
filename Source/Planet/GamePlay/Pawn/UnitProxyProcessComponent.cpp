
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

FName UUnitProxyProcessComponent::ComponentName = TEXT("InteractiveSkillComponent");

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
	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Update(WeaponSocket);
}

void UUnitProxyProcessComponent::RegisterMultiGAs(
	const TSharedPtr<FSkillSocket>& SkillSocket
)
{
	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Update(SkillSocket);
}

void UUnitProxyProcessComponent::ActiveWeapon()
{
	auto AllocationSkills = GetAllocationSkills();

	auto WeaponsMap = AllocationSkills->GetWeaponsMap();

	if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1];
		SwitchWeaponImp(WeaponSocketSPtr);
	}
	else if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2];
		SwitchWeaponImp(WeaponSocketSPtr);
	}
}

bool UUnitProxyProcessComponent::SwitchWeapon()
{
	auto AllocationSkills = GetAllocationSkills();

	auto WeaponsMap = AllocationSkills->GetWeaponsMap();
	if (
		(WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1)) ||
		(WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
		)
	{
		auto WeaponSocket1SPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket1];
		auto WeaponSocket2SPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2];
		if (WeaponSocket1SPtr == PreviousWeaponSocketSPtr)
		{
			SwitchWeaponImp(WeaponSocket2SPtr);

			return true;
		}
		else
		{
			SwitchWeaponImp(WeaponSocket1SPtr);

			return true;
		}
	}

	return false;
}

void UUnitProxyProcessComponent::RetractputWeapon()
{
	SwitchWeaponImp(nullptr);
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
	if (PreviousWeaponSocketSPtr)
	{
		return PreviousWeaponSocketSPtr->UnitPtr.Pin();
	}

	return nullptr;
}

TSharedPtr<FAllocationSkills> UUnitProxyProcessComponent::GetAllocationSkills() const
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return nullptr;
	}

	return OnwerActorPtr->GetCharacterUnit()->AllocationSkills;
}

bool UUnitProxyProcessComponent::ActiveAction(
	const TSharedPtr<FSocketBase>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */
)
{
	auto AllocationSkills = GetAllocationSkills();

	return AllocationSkills->Active(CanbeActivedInfoSPtr);
}

void UUnitProxyProcessComponent::CancelAction(const TSharedPtr<FSocketBase>& CanbeActivedInfoSPtr)
{
	auto AllocationSkills = GetAllocationSkills();

	AllocationSkills->Cancel(CanbeActivedInfoSPtr);
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
			SwitchWeaponImp(WeaponSocketSPtr);

			return true;
		}
	}
	else if (WeaponsMap.Contains(UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2))
	{
		auto WeaponSocketSPtr = WeaponsMap[UGameplayTagsSubSystem::GetInstance()->WeaponActiveSocket2];

		if (WeaponSocketSPtr->UnitPtr.Pin()->GetUnitType() == RequireWeaponUnitType)
		{
			SwitchWeaponImp(WeaponSocketSPtr);

			return true;
		}
	}

	return false;
}

void UUnitProxyProcessComponent::SwitchWeaponImp(const TSharedPtr<FWeaponSocket>& NewWeaponSocketSPtr)
{
	if (NewWeaponSocketSPtr == PreviousWeaponSocketSPtr)
	{
	}
	else
	{
		if (PreviousWeaponSocketSPtr)
		{
			PreviousWeaponSocketSPtr->UnitPtr.Pin()->RetractputWeapon();
		}
		if (NewWeaponSocketSPtr)
		{
			NewWeaponSocketSPtr->UnitPtr.Pin()->ActiveWeapon();
		}
		PreviousWeaponSocketSPtr = NewWeaponSocketSPtr;
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
