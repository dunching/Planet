
#include "SceneElement.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"

UBasicUnit::UBasicUnit()
{

}

UBasicUnit::~UBasicUnit()
{

}

UBasicUnit::IDType UBasicUnit::GetID()const
{
	return ID;
}

FGameplayTag UBasicUnit::GetUnitType() const
{
	return UnitType;
}

TSoftObjectPtr<UTexture2D> UBasicUnit::GetIcon() const
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit();

	return SceneUnitExtendInfoPtr->DefaultIcon;
}

FString UBasicUnit::GetUnitName() const
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit();

	return SceneUnitExtendInfoPtr->UnitName;
}

FTableRowUnit* UBasicUnit::GetTableRowUnit() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

UConsumableUnit::UConsumableUnit()
{

}

void UConsumableUnit::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 UConsumableUnit::GetCurrentValue() const
{
	return Num;
}

UToolUnit::UToolUnit()
{

}

int32 UToolUnit::GetNum() const
{
	return Num;
}

UWeaponUnit::UWeaponUnit()
{

}

FTableRowUnit_WeaponExtendInfo* UWeaponUnit::GetTableRowUnit_WeaponExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_WeaponExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_WeaponExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

USkillUnit::USkillUnit() :
	Super()
{

}

UWeaponSkillUnit::UWeaponSkillUnit()
{
}

UTalentSkillUnit::UTalentSkillUnit()
{
}

UActiveSkillUnit::UActiveSkillUnit()
{
}

FTableRowUnit_ActiveSkillExtendInfo* UActiveSkillUnit::GetTableRowUnit_ActiveSkillExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_ActiveSkillExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_ActiveSkillExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

UPassiveSkillUnit::UPassiveSkillUnit()
{
}

UGourpmateUnit::UGourpmateUnit()
{

}

void UGourpmateUnit::InitialByCharactor(FPawnType* InCharacterPtr)
{
	PCPtr = InCharacterPtr;
	Name = InCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name;
}

UCoinUnit::UCoinUnit()
{

}

void UCoinUnit::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 UCoinUnit::GetCurrentValue() const
{
	return Num;
}

UWeaponUnit* FSceneUnitContainer::AddUnit_Weapon(FGameplayTag UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = NewObject<UWeaponUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);

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

UGourpmateUnit* FSceneUnitContainer::AddUnit_Groupmate(FGameplayTag UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = NewObject<UGourpmateUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);

	const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

	ResultPtr->ID = NewID;
	ResultPtr->UnitType = UnitType;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(NewID, ResultPtr);

	OnGroupmateUnitChanged.ExcuteCallback(ResultPtr, true);

	return ResultPtr;
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

UBasicUnit* FSceneUnitContainer::FindUnit(UBasicUnit::IDType ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
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

FTableRowUnit* FSceneUnitContainer::GetTableRowUnit(FGameplayTag UnitType) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}
