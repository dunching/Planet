
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
#include "Skill_Base.h"

FBasicProxy::FBasicProxy()
{

}

FBasicProxy::~FBasicProxy()
{

}

void FBasicProxy::InitialUnit()
{
	ID = FGuid::NewGuid();
}

FBasicProxy::IDType FBasicProxy::GetID()const
{
	return ID;
}

FGameplayTag FBasicProxy::GetUnitType() const
{
	return UnitType;
}

TSoftObjectPtr<UTexture2D> FBasicProxy::GetIcon() const
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit();

	return SceneUnitExtendInfoPtr->DefaultIcon;
}

FString FBasicProxy::GetUnitName() const
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit();

	return SceneUnitExtendInfoPtr->UnitName;
}

void FBasicProxy::SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	if (AllocationCharacterUnitPtr == InAllocationCharacterUnitPtr)
	{
		return;
	}

	AllocationCharacterUnitPtr = InAllocationCharacterUnitPtr;

	OnAllocationCharacterUnitChanged.ExcuteCallback(nullptr);
}

TWeakPtr<FCharacterProxy> FBasicProxy::GetAllocationCharacterUnit() const
{
	return AllocationCharacterUnitPtr;
}

FTableRowUnit* FBasicProxy::GetTableRowUnit() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

FConsumableProxy::FConsumableProxy()
{

}

void FConsumableProxy::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 FConsumableProxy::GetCurrentValue() const
{
	return Num;
}

FTableRowUnit_Consumable* FConsumableProxy::GetTableRowUnit_Consumable() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_Consumable.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_Consumable>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

bool FConsumableProxy::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	auto MaxRemainingCooldown = -1.f;
	auto MaxRemainingCooldownPercent = -1.f;

	auto CurResult = true;
	auto CurRemainingCooldown = -1.f;
	auto CurRemainingCooldownPercent = -1.f;

	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
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

bool FConsumableProxy::CheckCooldown() const
{
	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
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

void FConsumableProxy::AddCooldownConsumeTime(float NewTime)
{
}

void FConsumableProxy::FreshUniqueCooldownTime()
{
	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->FreshCooldownTime();
	}
}

void FConsumableProxy::ApplyCooldown()
{
	AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyCooldown(
		this
	);
}

void FConsumableProxy::OffsetCooldownTime()
{
	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyCooldown(
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

FToolProxy::FToolProxy()
{

}

int32 FToolProxy::GetNum() const
{
	return Num;
}

FWeaponProxy::FWeaponProxy()
{

}

void FWeaponProxy::InitialUnit()
{
	Super::InitialUnit();
	{
		MaxAttackDistance = GetTableRowUnit_WeaponExtendInfo()->MaxAttackDistance;
	}
}

void FWeaponProxy::SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	Super::SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);

	FirstSkill->SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);
}

FTableRowUnit_WeaponExtendInfo* FWeaponProxy::GetTableRowUnit_WeaponExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_WeaponExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_WeaponExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

FTableRowUnit_PropertyEntrys* FWeaponProxy::GetMainPropertyEntry() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_PropertyEntrys.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_PropertyEntrys>(
		*GetTableRowUnit_WeaponExtendInfo()->PropertyEntry.ToString(), TEXT("GetUnit")
	);
	return SceneUnitExtendInfoPtr;
}

int32 FWeaponProxy::GetMaxAttackDistance() const
{
	return MaxAttackDistance;
}

FSkillProxy::FSkillProxy() :
	Super()
{

}

TSubclassOf<USkill_Base> FSkillProxy::GetSkillClass() const
{
	return nullptr;
}

void FSkillProxy::SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	if (!InAllocationCharacterUnitPtr)
	{
		UnRegisterSkill();
	}

	Super::SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);

	if (AllocationCharacterUnitPtr.IsValid())
	{
		RegisterSkill();
	}
}

void FSkillProxy::RegisterSkill()
{
	FGameplayAbilityTargetData_Skill* GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Skill;

	GameplayAbilityTargetDataPtr->SkillUnitPtr = nullptr;

	FGameplayAbilitySpec GameplayAbilitySpec(
		GetSkillClass(),
		Level
	);

	GameplayAbilitySpec.GameplayEventData = MakeShared<FGameplayEventData>();
	GameplayAbilitySpec.GameplayEventData->TargetData.Add(GameplayAbilityTargetDataPtr);

	auto ProxyCharacterPtr = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;
	GameplayAbilitySpecHandle = ProxyCharacterPtr->GetAbilitySystemComponent()->GiveAbility(GameplayAbilitySpec);
}

void FSkillProxy::UnRegisterSkill()
{
	if (GetAllocationCharacterUnit().IsValid())
	{
		auto ProxyCharacterPtr = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;

		if (ProxyCharacterPtr.IsValid())
		{
			auto ASCPtr = ProxyCharacterPtr->GetAbilitySystemComponent();

			ASCPtr->CancelAbilityHandle(GameplayAbilitySpecHandle);
			ASCPtr->ClearAbility(GameplayAbilitySpecHandle);
		}
	}

	GameplayAbilitySpecHandle = FGameplayAbilitySpecHandle();
}

USkill_Base* FSkillProxy::GetGAInst()const
{
	auto ProxyCharacterPtr = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;
	auto ASCPtr = ProxyCharacterPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
	if (GameplayAbilitySpecPtr)
	{
		return Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
	}

	return nullptr;
}

FGameplayAbilitySpecHandle FSkillProxy::GetGAHandle() const
{
	return GameplayAbilitySpecHandle;
}

FWeaponSkillProxy::FWeaponSkillProxy()
{
}

FTableRowUnit_WeaponSkillExtendInfo* FWeaponSkillProxy::GetTableRowUnit_WeaponSkillExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_WeaponSkillExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_WeaponSkillExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

TSubclassOf<USkill_Base> FWeaponSkillProxy::GetSkillClass() const
{
	return GetTableRowUnit_WeaponSkillExtendInfo()->SkillClass;
}

FTalentSkillProxy::FTalentSkillProxy()
{
}

FActiveSkillProxy::FActiveSkillProxy()
{
}

FTableRowUnit_ActiveSkillExtendInfo* FActiveSkillProxy::GetTableRowUnit_ActiveSkillExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_ActiveSkillExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_ActiveSkillExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

TSubclassOf<USkill_Base> FActiveSkillProxy::GetSkillClass() const
{
	return GetTableRowUnit_ActiveSkillExtendInfo()->SkillClass;
}

FPassiveSkillProxy::FPassiveSkillProxy()
{
}

void FPassiveSkillProxy::InitialUnit()
{
}

FTableRowUnit_PassiveSkillExtendInfo* FPassiveSkillProxy::GetTableRowUnit_PassiveSkillExtendInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_PassiveSkillExtendInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_PassiveSkillExtendInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

FTableRowUnit_PropertyEntrys* FPassiveSkillProxy::GetMainPropertyEntry() const
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

TSubclassOf<USkill_Base> FPassiveSkillProxy::GetSkillClass() const
{
	return GetTableRowUnit_PassiveSkillExtendInfo()->SkillClass;
}

FCharacterProxy::FCharacterProxy()
{
	CharacterAttributesSPtr = MakeShared<FCharacterAttributes>();
	AllocationSkills = MakeShared<FAllocationSkills>();
	SceneUnitContainer = MakeShared<FSceneUnitContainer>();
}

FTableRowUnit_CharacterInfo* FCharacterProxy::GetTableRowUnit_CharacterInfo() const
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit_CharacterInfo.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit_CharacterInfo>(*UnitType.ToString(), TEXT("GetUnit"));
	return SceneUnitExtendInfoPtr;
}

void FCharacterProxy::RelieveRootBind()
{
	SceneUnitContainer.Reset();
}

FCoinProxy::FCoinProxy()
{

}

void FCoinProxy::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 FCoinProxy::GetCurrentValue() const
{
	return Num;
}

bool FActiveSkillProxy::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	auto MaxRemainingCooldown = -1.f;
	auto MaxRemainingCooldownPercent = -1.f;

	auto CurResult = true;
	auto CurRemainingCooldown = -1.f;
	auto CurRemainingCooldownPercent = -1.f;

	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
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

bool FActiveSkillProxy::CheckCooldown() const
{
	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
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

void FActiveSkillProxy::AddCooldownConsumeTime(float NewTime)
{
	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	if (CooldownMap.Contains(GetUnitType()))
	{
		CooldownMap[GetUnitType()].Pin()->AddCooldownConsumeTime(NewTime);
	}
}

void FActiveSkillProxy::FreshUniqueCooldownTime()
{
	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->FreshCooldownTime();
	}
}

void FActiveSkillProxy::ApplyCooldown()
{
	AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyCooldown(
		this
	);
}

void FActiveSkillProxy::OffsetCooldownTime()
{
	AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->ApplyUniqueCooldown(
		this
	);

	auto CooldownMap = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetCooldown(
		this
	);

	// 获取当前技能CD
	if (CooldownMap.Contains(GetUnitType()) && CooldownMap[GetUnitType()].IsValid())
	{
		CooldownMap[GetUnitType()].Pin()->OffsetCooldownTime();
	}
}
