
#include "SceneElement.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "HumanControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"

UWeaponUnit* FSceneToolsContainer::AddUnit(EWeaponUnitType Type)
{
	auto AssetRefMapPtr = USceneUnitExtendInfoMap::GetInstance();

	UWeaponUnit* ResultPtr = NewObject<UWeaponUnit>(GetWorldImp(), AssetRefMapPtr->WeaponToolsMap[Type]);

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

			ResultPtr->FirstSkill = AddUnit(ResultPtr->FirstSkillClass);

			SceneToolsAry.Add(ResultPtr);
			SceneMetaMap.Add(NewID, ResultPtr);

			break;
		}
	}

	return ResultPtr;
}

USkillUnit* FSceneToolsContainer::AddUnit(ESkillUnitType Type)
{
	if (SkillUnitMap.Contains(Type))
	{
		// 

		return nullptr;
	}
	else
	{
		auto AssetRefMapPtr = USceneUnitExtendInfoMap::GetInstance();

		USkillUnit* ResultPtr = NewObject<USkillUnit>(GetWorldImp(), AssetRefMapPtr->SkillToolsMap[Type]);

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

UConsumableUnit* FSceneToolsContainer::AddUnit(EConsumableUnitType Type, int32 Num)
{
	check(Num > 0);

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

		UConsumableUnit* ResultPtr = NewObject<UConsumableUnit>(GetWorldImp(), AssetRefMapPtr->ConsumableToolMap[Type]);

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

UToolUnit* FSceneToolsContainer::AddUnit(EToolUnitType Type)
{
	auto AssetRefMapPtr = USceneUnitExtendInfoMap::GetInstance();

	UToolUnit* ResultPtr = NewObject<UToolUnit>(GetWorldImp(), AssetRefMapPtr->EquipmentToolsMap[Type]);

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

			break;
		}
	}

	return ResultPtr;
}

UCoinUnit* FSceneToolsContainer::AddUnit(ECoinUnitType Type, int32 Num /*= 1*/)
{
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

		auto ResultPtr = NewObject<UCoinUnit>(GetWorldImp(), AssetRefMapPtr->CoinToolMap[Type]);

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

void FSceneToolsContainer::RemoveUnit(UConsumableUnit* UnitPtr, int32 Num /*= 1*/)
{
	if (UnitPtr)
	{
		UnitPtr->Num -= Num;

		OnConsumableUnitChanged.ExcuteCallback(UnitPtr, false, Num);
	}
}

USkillUnit* FSceneToolsContainer::FindUnit(ESkillUnitType Type)
{
	for (const auto& Iter : SceneToolsAry)
	{
		if (!Iter)
		{
			continue;
		}
		switch (Iter->GetSceneToolsType())
		{
		case ESceneToolsType::kSkill:
		{
			if (Iter->GetSceneElementType<ESkillUnitType>() == Type)
			{
				return Cast<USkillUnit>(Iter);
			}
		}
		break;
		}
	}

	return nullptr;
}

UWeaponUnit* FSceneToolsContainer::FindUnit(EWeaponUnitType Type)
{
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
			if (Iter->GetSceneElementType<EWeaponUnitType>() == Type)
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

UCoinUnit* FSceneToolsContainer::FindUnit(ECoinUnitType Type)
{
	auto Iter = CoinUnitMap.Find(Type);
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

USkillUnit::USkillUnit() :
	Super(ESceneToolsType::kSkill)
{

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

UPassiveSkillUnit::UPassiveSkillUnit() :
	Super()
{
	SkillType = ESkillType::kActive;
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
