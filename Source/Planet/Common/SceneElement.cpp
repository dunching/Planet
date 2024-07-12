
#include "SceneElement.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "HumanControllerInterface.h"
#include "HumanCharacter.h"

UWeaponUnit* FSceneToolsContainer::AddUnit(EWeaponUnitType Type)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();

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
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();

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

			break;
		}
	}

	return ResultPtr;
}

UConsumableUnit* FSceneToolsContainer::AddUnit(EConsumableUnitType Type, int32 Num)
{
	check(Num > 0);

	auto AssetRefMapPtr = UAssetRefMap::GetInstance();

	if (ConsumablesUnitMap.Contains(Type))
	{
		auto Ref = ConsumablesUnitMap[Type];

		Ref->Num += Num;

		return Ref;
	}
	else
	{
		UConsumableUnit* ResultPtr = NewObject<UConsumableUnit>(GetWorldImp(), AssetRefMapPtr->ConsumableToolMap[Type]);

		const auto NewID = FMath::RandRange(1, std::numeric_limits<UBasicUnit::IDType>::max());

		ResultPtr->Num = Num;
		ResultPtr->ID = NewID;
		ResultPtr->UnitType = Type;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(NewID, ResultPtr);
		ConsumablesUnitMap.Add(Type, ResultPtr);

		return ResultPtr;
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

UToolUnit* FSceneToolsContainer::AddUnit(EToolUnitType Type)
{
	auto AssetRefMapPtr = UAssetRefMap::GetInstance();

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

UBasicUnit* FSceneToolsContainer::FindUnit(UBasicUnit::IDType ID)
{
	auto Iter = SceneMetaMap.Find(ID);
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