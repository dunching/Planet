
#include "SceneUnitContainer.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"

TSharedPtr<FWeaponProxy> FSceneUnitContainer::AddUnit_Weapon(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FWeaponProxy>();

#if WITH_EDITOR
	TestGCUnitMap.Add(ResultPtr);
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;
	ResultPtr->FirstSkill = AddUnit_Skill(ResultPtr->GetTableRowUnit_WeaponExtendInfo()->WeaponSkillUnitType);

	ResultPtr->InitialUnit();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponUnitChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> FSceneUnitContainer::AddUnit_Weapon(const FWeaponProxy& Unit)
{
	auto ResultPtr = MakeShared<FWeaponProxy>();

#if WITH_EDITOR
	TestGCUnitMap.Add(ResultPtr);
#endif

	* ResultPtr = Unit;
	ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponUnitChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> FSceneUnitContainer::FindUnit_Weapon(const FGameplayTag& UnitType)
{
	return nullptr;
}

TSharedPtr<FSkillProxy>  FSceneUnitContainer::AddUnit_Skill(const FGameplayTag& UnitType)
{
	if (SkillUnitMap.Contains(UnitType))
	{
		// 

		return SkillUnitMap[UnitType];
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

		auto ResultPtr = MakeShared<FSkillProxy>();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;

		ResultPtr->InitialUnit();

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		SkillUnitMap.Add(UnitType, ResultPtr);

		OnSkillUnitChanged(ResultPtr, true);

		return ResultPtr;
	}
}

TSharedPtr<FSkillProxy>  FSceneUnitContainer::AddUnit_Skill(const FSkillProxy& Unit)
{
	if (SkillUnitMap.Contains(Unit.UnitType))
	{
		// 

		return SkillUnitMap[Unit.UnitType];
	}
	else
	{
		auto ResultPtr = MakeShared<FSkillProxy>();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		* ResultPtr = Unit;
		ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		SkillUnitMap.Add(Unit.UnitType, ResultPtr);

		OnSkillUnitChanged(ResultPtr, true);

		return ResultPtr;
	}
}

TSharedPtr<FSkillProxy> FSceneUnitContainer::FindUnit_Skill(const FGameplayTag& UnitType)
{
	auto Iter = SkillUnitMap.Find(UnitType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

TSharedPtr<FSkillProxy> FSceneUnitContainer::FindUnit_Skill(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FSkillProxy>(SceneMetaMap[ID]);
	}
	return nullptr;
}

TSharedPtr<FCharacterProxy> FSceneUnitContainer::AddUnit_Character(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FCharacterProxy>();

#if WITH_EDITOR
	TestGCUnitMap.Add(ResultPtr);
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	return ResultPtr;
}

void FSceneUnitContainer::AddUnit_Groupmate(const TSharedPtr<FCharacterProxy>& UnitPtr)
{
	SceneToolsAry.Add(UnitPtr);
	SceneMetaMap.Add(UnitPtr->ID, UnitPtr);

	OnGroupmateUnitChanged.ExcuteCallback(UnitPtr, true);
}

TSharedPtr <FConsumableProxy> FSceneUnitContainer::AddUnit_Consumable(const FGameplayTag& UnitType, int32 Num)
{
	check(Num > 0);

	if (ConsumablesUnitMap.Contains(UnitType))
	{
		auto Ref = ConsumablesUnitMap[UnitType];

		Ref->Num += Num;

		OnConsumableUnitChanged.ExcuteCallback(Ref, true, Num);

		return Ref;
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

		auto ResultPtr = MakeShared<FConsumableProxy>();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		ResultPtr->Num = Num;
		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		ConsumablesUnitMap.Add(UnitType, ResultPtr);

		OnConsumableUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

TSharedPtr<FToolProxy> FSceneUnitContainer::AddUnit_ToolUnit(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FToolProxy>();

#if WITH_EDITOR
	TestGCUnitMap.Add(ResultPtr);
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	return ResultPtr;
}

FSceneUnitContainer::~FSceneUnitContainer()
{
	for (auto Iter : SceneToolsAry)
	{
		if (Iter)
		{
			if (TestGCUnitMap.Contains(Iter))
			{
				TestGCUnitMap.Remove(Iter);
			}

		}
	}
}

TSharedPtr<FBasicProxy> FSceneUnitContainer::AddUnit(const FGameplayTag& UnitType, int32 Num)
{
	TSharedPtr<FBasicProxy> Result = nullptr;

	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
		Result = AddUnit_ToolUnit(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		Result = AddUnit_Weapon(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill))
	{
		Result = AddUnit_Skill(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
		Result = AddUnit_Coin(UnitType, Num);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
		Result = AddUnit_Consumable(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->DataSource_Character))
	{
		Result = AddUnit_Consumable(UnitType);
	}

	return Result;
}

TSharedPtr<FBasicProxy> FSceneUnitContainer::FindUnit(IDType ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

void FSceneUnitContainer::RemoveUnit(IDType ID)
{
	auto TargetIter = SceneMetaMap.Find(ID);

	if (TargetIter)
	{
		SceneMetaMap.Remove(ID);
	}

	for (auto Iter : SceneToolsAry)
	{
		if (Iter == *TargetIter)
		{
			SceneToolsAry.Remove(Iter);
			break;
		}
	}
}

TSharedPtr<FCoinProxy> FSceneUnitContainer::AddUnit_Coin(const FGameplayTag& UnitType, int32 Num /*= 1*/)
{
	if (CoinUnitMap.Contains(UnitType))
	{
		auto Ref = CoinUnitMap[UnitType];

		Ref->Num += Num;

		OnCoinUnitChanged.ExcuteCallback(Ref, true, Num);

		return Ref;
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

		auto ResultPtr = MakeShared<FCoinProxy>();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacterUnitPtr = OwnerCharacter;
		ResultPtr->Num = Num;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		CoinUnitMap.Add(UnitType, ResultPtr);

		OnCoinUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

TSharedPtr<FCoinProxy> FSceneUnitContainer::FindUnit_Coin(const FGameplayTag& UnitType)
{
	auto Iter = CoinUnitMap.Find(UnitType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

void FSceneUnitContainer::RemoveUnit_Consumable(const TSharedPtr<FConsumableProxy>& UnitPtr, int32 Num /*= 1*/)
{
	if (UnitPtr)
	{
		UnitPtr->Num -= Num;

		OnConsumableUnitChanged.ExcuteCallback(UnitPtr, false, Num);
	}
}

const TArray<TSharedPtr<FBasicProxy>>& FSceneUnitContainer::GetSceneUintAry() const
{
	return SceneToolsAry;
}

const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& FSceneUnitContainer::GetCoinUintAry() const
{
	return CoinUnitMap;
}

TArray<TSharedPtr<FCharacterProxy>> FSceneUnitContainer::GetGourpmateUintAry() const
{
	TArray<TSharedPtr<FCharacterProxy>> Result;

	for (auto Iter : SceneToolsAry)
	{
		if (Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
		{
			auto GroupmateUnitPtr = DynamicCastSharedPtr<FCharacterProxy>(Iter);
			check(GroupmateUnitPtr);
			if (GroupmateUnitPtr)
			{
				Result.Add(GroupmateUnitPtr);
			}
		}
	}

	return Result;
}

TSharedPtr<FCharacterProxy> FSceneUnitContainer::FindUnit_Character(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FCharacterProxy>(SceneMetaMap[ID]);
	}
	return nullptr;
}

int32 FSceneUnitContainer::GetValidID() const
{
	int32 Result = 0;

	return Result;
}

FTableRowUnit* GetTableRowUnit(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}
