
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
#include "GroupMnaggerComponent.h"
#include "PropertyEntrys.h"

UBasicUnit::UBasicUnit()
{

}

UBasicUnit::~UBasicUnit()
{

}

void UBasicUnit::InitialUnit()
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

void UBasicUnit::SetAllocationCharacterUnit(UCharacterUnit* InAllocationCharacterUnitPtr)
{
	if (AllocationCharacterUnitPtr == InAllocationCharacterUnitPtr)
	{
		return;
	}

	AllocationCharacterUnitPtr = InAllocationCharacterUnitPtr;

	OnAllocationCharacterUnitChanged.ExcuteCallback(AllocationCharacterUnitPtr);
}

UCharacterUnit* UBasicUnit::GetAllocationCharacterUnit() const
{
	return AllocationCharacterUnitPtr;
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

FTableRowUnit_Consumable* UConsumableUnit::GetTableRowUnit_Consumable() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_Consumable.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_Consumable>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

bool UConsumableUnit::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	auto MaxRemainingCooldown = -1.f;
	auto MaxRemainingCooldownPercent = -1.f;

	auto CurResult = true;
	auto CurRemainingCooldown = -1.f;
	auto CurRemainingCooldownPercent = -1.f;

	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	for (auto Iter : CooldownMap)
	{
		if (Iter.Value.IsValid())
		{
			CurResult = Iter.Value.Pin()->GetRemainingCooldown(CurRemainingCooldown, CurRemainingCooldownPercent);
			if (CurResult)
			{
				continue;
			}

			if (CurRemainingCooldown > MaxRemainingCooldown)
			{
				MaxRemainingCooldown = CurRemainingCooldown;
				MaxRemainingCooldownPercent = CurRemainingCooldownPercent;
			}
		}
	}

	RemainingCooldown = MaxRemainingCooldown;
	RemainingCooldownPercent = MaxRemainingCooldownPercent;

	return CurResult;
}

bool UConsumableUnit::CheckCooldown() const
{
	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	for (auto Iter : CooldownMap)
	{
		if (Iter.Value.IsValid())
		{
			if (!Iter.Value.Pin()->CheckCooldown())
			{
				return false;
			}
		}
	}

	return true;
}

void UConsumableUnit::AddCooldownConsumeTime(float NewTime)
{
}

void UConsumableUnit::FreshUniqueCooldownTime()
{
	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->FreshCooldownTime();
	}
}

void UConsumableUnit::ApplyCooldown()
{
	AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyCooldown(
		this
	);
}

void UConsumableUnit::OffsetCooldownTime()
{
	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->OffsetCooldownTime();
	}
}

FTableRowUnit_CommonCooldownInfo* GetTableRowUnit_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag)
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_CommonCooldownInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_CommonCooldownInfo>(*CommonCooldownTag.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
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

TSubclassOf<USkill_Base> USkillUnit::GetSkillClass() const
{
	return nullptr;
}

UWeaponSkillUnit::UWeaponSkillUnit()
{
}

FTableRowUnit_WeaponSkillExtendInfo* UWeaponSkillUnit::GetTableRowUnit_WeaponSkillExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_WeaponSkillExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_WeaponSkillExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

TSubclassOf<USkill_Base> UWeaponSkillUnit::GetSkillClass() const
{
	return GetTableRowUnit_WeaponSkillExtendInfo()->SkillClass;
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

TSubclassOf<USkill_Base> UActiveSkillUnit::GetSkillClass() const
{
	return GetTableRowUnit_ActiveSkillExtendInfo()->SkillClass;
}

UPassiveSkillUnit::UPassiveSkillUnit()
{
}

void UPassiveSkillUnit::InitialUnit()
{
}

FTableRowUnit_PassiveSkillExtendInfo* UPassiveSkillUnit::GetTableRowUnit_PassiveSkillExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_PassiveSkillExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_PassiveSkillExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

FTableRowUnit_PropertyEntrys* UPassiveSkillUnit::GetMainPropertyEntry() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_PropertyEntrys.LoadSynchronous();

	// 
	TArray<FTableRowUnit_PropertyEntrys*>ResultAry;
	DataTable->GetAllRows(TEXT("GetUnit"), ResultAry);
	if (!ResultAry.IsEmpty())
	{
		return ResultAry[FMath::RandRange(0, ResultAry.Num() - 1)];
	}

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_PropertyEntrys>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

TSubclassOf<USkill_Base> UPassiveSkillUnit::GetSkillClass() const
{
	return GetTableRowUnit_PassiveSkillExtendInfo()->SkillClass;
}

UCharacterUnit::UCharacterUnit()
{
	CharacterAttributesSPtr = MakeShared<FCharacterAttributes>();
	AllocationSkills = MakeShared<FAllocationSkills>();
	SceneUnitContainer = MakeShared<FSceneUnitContainer>();
}

FTableRowUnit_CharacterInfo* UCharacterUnit::GetTableRowUnit_CharacterInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_CharacterInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_CharacterInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

void UCharacterUnit::RelieveRootBind()
{
	SceneUnitContainer.Reset();
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

bool UActiveSkillUnit::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	auto MaxRemainingCooldown = -1.f;
	auto MaxRemainingCooldownPercent = -1.f;

	auto CurResult = true;
	auto CurRemainingCooldown = -1.f;
	auto CurRemainingCooldownPercent = -1.f;

	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	for (auto Iter : CooldownMap)
	{
		if (Iter.Value.IsValid())
		{
			CurResult = Iter.Value.Pin()->GetRemainingCooldown(CurRemainingCooldown, CurRemainingCooldownPercent);
			if (CurResult)
			{
				continue;
			}

			if (CurRemainingCooldown > MaxRemainingCooldown)
			{
				MaxRemainingCooldown = CurRemainingCooldown;
				MaxRemainingCooldownPercent = CurRemainingCooldownPercent;
			}
		}
	}

	RemainingCooldown = MaxRemainingCooldown;
	RemainingCooldownPercent = MaxRemainingCooldownPercent;

	return CurResult;
}

bool UActiveSkillUnit::CheckCooldown() const
{
	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	for (auto Iter : CooldownMap)
	{
		if (Iter.Value.IsValid())
		{
			if (!Iter.Value.Pin()->CheckCooldown())
			{
				return false;
			}
		}
	}

	return true;
}

void UActiveSkillUnit::AddCooldownConsumeTime(float NewTime)
{
	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	if (CooldownMap.Contains(GetUnitType()))
	{
		CooldownMap[GetUnitType()].Pin()->AddCooldownConsumeTime(NewTime);
	}
}

void UActiveSkillUnit::FreshUniqueCooldownTime()
{
	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->FreshCooldownTime();
	}
}

void UActiveSkillUnit::ApplyCooldown()
{
	AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyCooldown(
		this
	);
}

void UActiveSkillUnit::OffsetCooldownTime()
{
	AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyUniqueCooldown(
		this
	);

	auto CooldownMap = AllocationCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->OffsetCooldownTime();
	}
}
