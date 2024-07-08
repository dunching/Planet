
#include "SceneElement.h"

#include "AssetRefMap_SceneObjects.h"
#include "Tools.h"

UWeaponUnit* FSceneToolsContainer::AddUnit(EWeaponUnitType Type)
{
	auto AssetRefMapPtr = UAssetRefMap_SceneObjects::GetInstance();

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
	auto AssetRefMapPtr = UAssetRefMap_SceneObjects::GetInstance();

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
	auto AssetRefMapPtr = UAssetRefMap_SceneObjects::GetInstance();

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
//	Name = InCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name;
}

UPassiveSkillUnit::UPassiveSkillUnit() :
	Super()
{
	SkillType = ESkillType::kActive;
}
