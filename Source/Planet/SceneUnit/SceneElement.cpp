
#include "SceneElement.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
#include "CharacterAttibutes.h"
#include "AllocationSkills.h"
#include "SceneUnitContainer.h"

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

UCharacterUnit::UCharacterUnit()
{
	CharacterAttributes = MakeShared<FCharacterAttributes>();
	AllocationSkills = MakeShared<FAllocationSkills>();
	SceneUnitContainer = MakeShared<FSceneUnitContainer>();
}

void UCharacterUnit::InitialByCharactor(FPawnType* InCharacterPtr)
{
	ProxyCharacterPtr = InCharacterPtr;
	CharacterAttributes->Name = InCharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().Name;
}

FTableRowUnit_CharacterInfo* UCharacterUnit::GetTableRowUnit_CharacterInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_CharacterInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_CharacterInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
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
