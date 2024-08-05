
#include "SceneUnitContainer.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"

UWeaponUnit* FSceneUnitContainer::AddUnit_Weapon(FGameplayTag UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = NewObject<UWeaponUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);
	ResultPtr->AddToRoot();

#if WITH_EDITOR
	TestGCUnitMap.Add(ResultPtr);
#endif

	for (;;)
	{
		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());
		if (SceneMetaMap.Contains(NewID))
		{
			continue;
		}
		else
		{
			ResultPtr->ID = NewID;
			ResultPtr->UnitType = UnitType;

			ResultPtr->FirstSkill = AddUnit_Skill(ResultPtr->GetTableRowUnit_WeaponExtendInfo()->WeaponSkillUnitType);

			SceneToolsAry.Add(ResultPtr);
			SceneMetaMap.Add(NewID, ResultPtr);

			break;
		}
	}

	return ResultPtr;
}

UWeaponUnit* FSceneUnitContainer::FindUnit_Weapon(FGameplayTag UnitType)
{
	return nullptr;
}

USkillUnit* FSceneUnitContainer::AddUnit_Skill(FGameplayTag UnitType)
{
	if (SkillUnitMap.Contains(UnitType))
	{
		// 

		return nullptr;
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

		auto ResultPtr = NewObject<USkillUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);
		ResultPtr->AddToRoot();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		if (!ResultPtr)
		{
			return nullptr;
		}

		for (;;)
		{
			const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());
			if (SceneMetaMap.Contains(NewID))
			{
				continue;
			}
			else
			{
				ResultPtr->ID = NewID;
				ResultPtr->UnitType = UnitType;

				SceneToolsAry.Add(ResultPtr);
				SceneMetaMap.Add(NewID, ResultPtr);
				SkillUnitMap.Add(UnitType, ResultPtr);

				OnSkillUnitChanged.ExcuteCallback(ResultPtr, true);

				break;
			}
		}
		return ResultPtr;
	}
}

USkillUnit* FSceneUnitContainer::FindUnit_Skill(FGameplayTag UnitType)
{
	auto Iter = SkillUnitMap.Find(UnitType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

UCharacterUnit* FSceneUnitContainer::AddUnit_Groupmate(FGameplayTag UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = NewObject<UCharacterUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);
//	ResultPtr->AddToRoot();

	const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

	ResultPtr->ID = NewID;
	ResultPtr->UnitType = UnitType;

	AddUnit_Groupmate(ResultPtr);

	return ResultPtr;
}

void FSceneUnitContainer::AddUnit_Groupmate(UCharacterUnit* UnitPtr)
{
	SceneToolsAry.Add(UnitPtr);
	SceneMetaMap.Add(UnitPtr->ID, UnitPtr);

	OnGroupmateUnitChanged.ExcuteCallback(UnitPtr, true);
}

UConsumableUnit* FSceneUnitContainer::AddUnit_Consumable(FGameplayTag UnitType, int32 Num)
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

		auto ResultPtr = NewObject<UConsumableUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);
		ResultPtr->AddToRoot();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

		ResultPtr->Num = Num;
		ResultPtr->ID = NewID;
		ResultPtr->UnitType = UnitType;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(NewID, ResultPtr);
		ConsumablesUnitMap.Add(UnitType, ResultPtr);

		OnConsumableUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

UToolUnit* FSceneUnitContainer::AddUnit_ToolUnit(FGameplayTag UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = NewObject<UToolUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);
	ResultPtr->AddToRoot();

#if WITH_EDITOR
	TestGCUnitMap.Add(ResultPtr);
#endif

	for (;;)
	{
		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());
		if (SceneMetaMap.Contains(NewID))
		{
			continue;
		}
		else
		{
			ResultPtr->ID = NewID;
			ResultPtr->UnitType = UnitType;

			SceneToolsAry.Add(ResultPtr);
			SceneMetaMap.Add(NewID, ResultPtr);

			break;
		}
	}

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

			Iter->RemoveFromRoot();
		}
	}
}

UBasicUnit* FSceneUnitContainer::AddUnit(FGameplayTag UnitType, int32 Num)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
		return AddUnit_ToolUnit(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		return AddUnit_Weapon(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill))
	{
		return AddUnit_Skill(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
		return AddUnit_Coin(UnitType, Num);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
		return AddUnit_Consumable(UnitType);
	}

	return nullptr;
}

UBasicUnit* FSceneUnitContainer::FindUnit(int32 ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

void FSceneUnitContainer::RemoveUnit(int32 ID)
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

UCoinUnit* FSceneUnitContainer::AddUnit_Coin(FGameplayTag UnitType, int32 Num /*= 1*/)
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

		auto ResultPtr = NewObject<UCoinUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);
		ResultPtr->AddToRoot();

#if WITH_EDITOR
		TestGCUnitMap.Add(ResultPtr);
#endif

		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

		ResultPtr->Num = Num;
		ResultPtr->ID = NewID;
		ResultPtr->UnitType = UnitType;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(NewID, ResultPtr);
		CoinUnitMap.Add(UnitType, ResultPtr);

		OnCoinUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

UCoinUnit* FSceneUnitContainer::FindUnit_Coin(FGameplayTag UnitType)
{
	auto Iter = CoinUnitMap.Find(UnitType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

void FSceneUnitContainer::RemoveUnit_Consumable(UConsumableUnit* UnitPtr, int32 Num /*= 1*/)
{
	if (UnitPtr)
	{
		UnitPtr->Num -= Num;

		OnConsumableUnitChanged.ExcuteCallback(UnitPtr, false, Num);
	}
}

const TArray<UBasicUnit*>& FSceneUnitContainer::GetSceneUintAry() const
{
	return SceneToolsAry;
}

const TMap<FGameplayTag, UCoinUnit*>& FSceneUnitContainer::GetCoinUintAry() const
{
	return CoinUnitMap;
}

TArray<UCharacterUnit*> FSceneUnitContainer::GetGourpmateUintAry() const
{
	TArray<UCharacterUnit*> Result;

	for (auto Iter : SceneToolsAry)
	{
		if (Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
		{
			auto GroupmateUnitPtr = Cast<UCharacterUnit>(Iter);
			check(GroupmateUnitPtr);
			if (GroupmateUnitPtr)
			{
				Result.Add(GroupmateUnitPtr);
			}
		}
	}

	return Result;
}

FTableRowUnit* FSceneUnitContainer::GetTableRowUnit(FGameplayTag UnitType) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}
