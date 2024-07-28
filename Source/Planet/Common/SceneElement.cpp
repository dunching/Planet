
#include "SceneElement.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "HumanControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"

UBasicUnit::UBasicUnit(ESceneToolsType InSceneToolsType) :
	SceneToolsType(InSceneToolsType)
{

}

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

ESceneToolsType UBasicUnit::GetSceneToolsType() const
{
	return SceneToolsType;
}

TSoftObjectPtr<UTexture2D> UBasicUnit::GetIcon() const
{
	return DefaultIcon;
}

FString UBasicUnit::GetUnitName() const
{
	return UnitName;
}

UConsumableUnit::UConsumableUnit() :
	Super(ESceneToolsType::kConsumables)
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

UToolUnit::UToolUnit() :
	Super(ESceneToolsType::kTool)
{

}

int32 UToolUnit::GetNum() const
{
	return Num;
}

UWeaponUnit::UWeaponUnit() :
	Super(ESceneToolsType::kWeapon)
{

}

USkillUnit::USkillUnit(ESceneToolsType InSceneToolsType) :
	Super(InSceneToolsType)
{

}

USkillUnit::USkillUnit()
{

}

UWeaponSkillUnit::UWeaponSkillUnit() :
	Super(ESceneToolsType::kWeaponSkill)
{
	SkillType = ESkillType::kWeapon;
}

UTalentSkillUnit::UTalentSkillUnit() :
	Super(ESceneToolsType::kTalentSkill)
{
	SkillType = ESkillType::kTalent;
}

UActiveSkillUnit::UActiveSkillUnit() :
	Super(ESceneToolsType::kActiveSkill)
{
	SkillType = ESkillType::kActive;
}

UPassiveSkillUnit::UPassiveSkillUnit() :
	Super(ESceneToolsType::kPassveSkill)
{
	SkillType = ESkillType::kPassive;
}

UGourpmateUnit::UGourpmateUnit() :
	Super(ESceneToolsType::kGroupMate)
{

}

void UGourpmateUnit::InitialByCharactor(FPawnType* InCharacterPtr)
{
	PCPtr = InCharacterPtr;
	Name = InCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name;
}

UCoinUnit::UCoinUnit() :
	Super(ESceneToolsType::kCoin)
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

UWeaponUnit* FSceneToolsContainer::AddUnit_Weapon(FGuid UnitGuid)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);
	const auto Type = SceneUnitExtendInfoPtr->WeaponUnitType;

	UWeaponUnit* ResultPtr = NewObject<UWeaponUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);

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
			ResultPtr->UnitType = Type;

			ResultPtr->FirstSkill = GetUnitByType(ResultPtr->FirstSkillClass);

			SceneToolsAry.Add(ResultPtr);
			SceneMetaMap.Add(NewID, ResultPtr);

			break;
		}
	}

	return ResultPtr;
}

USkillUnit* FSceneToolsContainer::AddUnit_Skill(FGuid UnitGuid)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);
	const auto Type = SceneUnitExtendInfoPtr->SkillUnitType;

	return AddUnit_Skill(Type);
}

USkillUnit* FSceneToolsContainer::AddUnit_Skill(ESkillUnitType SkillUnitType)
{
	const auto Type = SkillUnitType;

	if (SkillUnitMap.Contains(Type))
	{
		// 

		return nullptr;
	}
	else
	{
		auto AssetRefMapPtr = USceneUnitExtendInfoMap::GetInstance();

		auto ResultPtr = GetUnitByType(Type);
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
				ResultPtr->UnitType = Type;

				SceneToolsAry.Add(ResultPtr);
				SceneMetaMap.Add(NewID, ResultPtr);
				SkillUnitMap.Add(Type, ResultPtr);

				OnSkillUnitChanged.ExcuteCallback(ResultPtr, true);

				break;
			}
		}
		return ResultPtr;
	}
}

USkillUnit* FSceneToolsContainer::FindUnit_Skill(FGuid UnitGuid)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);
	const auto Type = SceneUnitExtendInfoPtr->SkillUnitType;

	return FindUnit_Skill(Type);
}

USkillUnit* FSceneToolsContainer::FindUnit_Skill(ESkillUnitType SkillUnitType)
{
	auto Iter = SkillUnitMap.Find(SkillUnitType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

UConsumableUnit* FSceneToolsContainer::AddUnit_Consumable(FGuid UnitGuid, int32 Num)
{
	check(Num > 0);

	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);
	const auto Type = SceneUnitExtendInfoPtr->ConsumableUnitType;

	return AddUnit_Consumable(Type);
}

UConsumableUnit* FSceneToolsContainer::AddUnit_Consumable(EConsumableUnitType ConsumableUnitType, int32 Num /*= 1*/)
{
	const auto Type = ConsumableUnitType;

	if (ConsumablesUnitMap.Contains(Type))
	{
		auto Ref = ConsumablesUnitMap[Type];

		Ref->Num += Num;

		OnConsumableUnitChanged.ExcuteCallback(Ref, true, Num);

		return Ref;
	}
	else
	{
		auto AssetRefMapPtr = USceneUnitExtendInfoMap::GetInstance();

		auto ResultPtr = GetUnitByType(Type);

		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

		ResultPtr->Num = Num;
		ResultPtr->ID = NewID;
		ResultPtr->UnitType = Type;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(NewID, ResultPtr);
		ConsumablesUnitMap.Add(Type, ResultPtr);

		OnConsumableUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

UToolUnit* FSceneToolsContainer::AddUnit_ToolUnit(FGuid UnitGuid)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);

	UToolUnit* ResultPtr = NewObject<UToolUnit>(GetWorldImp(), SceneUnitExtendInfoPtr->UnitClass);

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
			ResultPtr->UnitType = SceneUnitExtendInfoPtr->ToolUnitType;

			SceneToolsAry.Add(ResultPtr);
			SceneMetaMap.Add(NewID, ResultPtr);

			break;
		}
	}

	return ResultPtr;
}

UBasicUnit* FSceneToolsContainer::AddUnit(FGuid UnitGuid, int32 Num)
{
	return nullptr;
}

UCoinUnit* FSceneToolsContainer::AddUnit_Coin(FGuid UnitGuid, int32 Num /*= 1*/)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);
	const auto Type = SceneUnitExtendInfoPtr->CoinUnitType;

	return AddUnit_Coin(Type, Num);
}

UCoinUnit* FSceneToolsContainer::AddUnit_Coin(ECoinUnitType CoinUnitType, int32 Num)
{
	const auto Type = CoinUnitType;

	if (CoinUnitMap.Contains(Type))
	{
		auto Ref = CoinUnitMap[Type];

		Ref->Num += Num;

		OnCoinUnitChanged.ExcuteCallback(Ref, true, Num);

		return Ref;
	}
	else
	{
		auto AssetRefMapPtr = USceneUnitExtendInfoMap::GetInstance();

		auto ResultPtr = GetUnitByType(CoinUnitType);

		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

		ResultPtr->Num = Num;
		ResultPtr->ID = NewID;
		ResultPtr->UnitType = Type;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(NewID, ResultPtr);
		CoinUnitMap.Add(Type, ResultPtr);

		OnCoinUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

void FSceneToolsContainer::AddUnit_Apending(FGuid UnitGuid, FGuid Guid)
{
	if (SkillUnitApendingMap.Contains(Guid))
	{
		SkillUnitApendingMap[Guid].Get<1>()++;
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);

		SkillUnitApendingMap.Add(Guid, TTuple<FGuid, int32>{ UnitGuid, 1});
	}
}

void FSceneToolsContainer::SyncApendingUnit(FGuid Guid)
{
	if (SkillUnitApendingMap.Contains(Guid))
	{
		for (const auto& Iter : SkillUnitApendingMap)
		{
			AddUnit(Iter.Value.Get<0>(), 1);
		}
		SkillUnitApendingMap.Remove(Guid);
	}
}

void FSceneToolsContainer::RemoveUnit_Consumable(UConsumableUnit* UnitPtr, int32 Num /*= 1*/)
{
	if (UnitPtr)
	{
		UnitPtr->Num -= Num;

		OnConsumableUnitChanged.ExcuteCallback(UnitPtr, false, Num);
	}
}

UWeaponUnit* FSceneToolsContainer::FindUnit_Weapon(FGuid UnitGuid)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);

	for (const auto& Iter : SceneToolsAry)
	{
		if (!Iter)
		{
			continue;
		}
		switch (Iter->GetSceneToolsType())
		{
		case ESceneToolsType::kWeapon:
		{
			if (Iter->GetSceneElementType<EWeaponUnitType>() == SceneUnitExtendInfoPtr->WeaponUnitType)
			{
				return Cast<UWeaponUnit>(Iter);
			}
		}
		break;
		}
	}

	return nullptr;
}

UBasicUnit* FSceneToolsContainer::FindUnit(UBasicUnit::IDType ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

UCoinUnit* FSceneToolsContainer::FindUnit_Coin(FGuid UnitGuid)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitGuid);

	return FindUnit_Coin(SceneUnitExtendInfoPtr->CoinUnitType);
}

UCoinUnit* FSceneToolsContainer::FindUnit_Coin(ECoinUnitType CoinUnitType)
{
	auto Iter = CoinUnitMap.Find(CoinUnitType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

const TArray<UBasicUnit*>& FSceneToolsContainer::GetSceneUintAry() const
{
	return SceneToolsAry;
}

const TMap<ECoinUnitType, UCoinUnit*>& FSceneToolsContainer::GetCoinUintAry() const
{
	return CoinUnitMap;
}

FTableRowUnit* FSceneToolsContainer::GetTableRowUnit(FGuid UnitGuid) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable.LoadSynchronous();
	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitGuid.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

USkillUnit* FSceneToolsContainer::GetUnitByType(ESkillUnitType SkillUnitType) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable.LoadSynchronous();

	TArray<FTableRowUnit*> OutRowArray;
	DataTable->GetAllRows<FTableRowUnit>(TEXT("GetUnit"), OutRowArray);

	for (const auto& Iter : OutRowArray)
	{
		if (Iter->SkillUnitType == SkillUnitType)
		{
			auto ResultPtr = NewObject<USkillUnit>(GetWorldImp(), Iter->UnitClass);

			return ResultPtr;
		}
	}

	return nullptr;
}

UWeaponUnit* FSceneToolsContainer::GetUnitByType(EWeaponUnitType WeaponUnitType) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable.LoadSynchronous();

	TArray<FTableRowUnit*> OutRowArray;
	DataTable->GetAllRows<FTableRowUnit>(TEXT("GetUnit"), OutRowArray);

	for (const auto& Iter : OutRowArray)
	{
		if (Iter->WeaponUnitType == WeaponUnitType)
		{
			auto ResultPtr = NewObject<UWeaponUnit>(GetWorldImp(), Iter->UnitClass);

			return ResultPtr;
		}
	}

	return nullptr;
}

UCoinUnit* FSceneToolsContainer::GetUnitByType(ECoinUnitType CoinUnitType) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable.LoadSynchronous();

	TArray<FTableRowUnit*> OutRowArray;
	DataTable->GetAllRows<FTableRowUnit>(TEXT("GetUnit"), OutRowArray);

	for (const auto& Iter : OutRowArray)
	{
		if (Iter->CoinUnitType == CoinUnitType)
		{
			auto ResultPtr = NewObject<UCoinUnit>(GetWorldImp(), Iter->UnitClass);

			return ResultPtr;
		}
	}

	return nullptr;
}

UConsumableUnit* FSceneToolsContainer::GetUnitByType(EConsumableUnitType ConsumableUnitType) const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable.LoadSynchronous();

	TArray<FTableRowUnit*> OutRowArray;
	DataTable->GetAllRows<FTableRowUnit>(TEXT("GetUnit"), OutRowArray);

	for (const auto& Iter : OutRowArray)
	{
		if (Iter->ConsumableUnitType == ConsumableUnitType)
		{
			auto ResultPtr = NewObject<UConsumableUnit>(GetWorldImp(), Iter->UnitClass);

			return ResultPtr;
		}
	}

	return nullptr;
}
