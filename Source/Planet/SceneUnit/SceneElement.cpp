
#include "SceneElement.h"

#include "AbilitySystemComponent.h"

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
#include "Weapon_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_Active_Control.h"
#include "Skill_WeaponActive_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "HoldingItemsComponent.h"
#include "CDCaculatorComponent.h"
#include "Skill_Consumable_Generic.h"

FBasicProxy::FBasicProxy()
{

}

FBasicProxy::~FBasicProxy()
{

}

bool FBasicProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << UnitType;
	Ar << ID;

	if (Ar.IsSaving())
	{
		bool bIsValid = AllocationCharacterUnitPtr.IsValid();
		Ar << bIsValid;
		if (bIsValid)
		{
			AllocationCharacterUnitPtr.Pin()->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		bool bIsValid = false;
		Ar << bIsValid;
		if (bIsValid)
		{
			auto TempPtr = MakeShared<FCharacterProxy>();
			TempPtr->NetSerialize(Ar, Map, bOutSuccess);

			AllocationCharacterUnitPtr =
				TempPtr->ProxyCharacterPtr->GetHoldingItemsComponent()->FindUnit_Character(TempPtr->GetID());
		}
	}

	if (Ar.IsSaving())
	{
		bool bIsValid = OwnerCharacterUnitPtr.IsValid();
		Ar << bIsValid;
		if (bIsValid)
		{
			OwnerCharacterUnitPtr.Pin()->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		bool bIsValid = false;
		Ar << bIsValid;
		if (bIsValid)
		{
			auto TempPtr = MakeShared<FCharacterProxy>();
			TempPtr->NetSerialize(Ar, Map, bOutSuccess);

			OwnerCharacterUnitPtr =
				TempPtr->ProxyCharacterPtr->GetHoldingItemsComponent()->FindUnit_Character(TempPtr->GetID());
		}
	}

	return true;
}

void FBasicProxy::InitialUnit()
{
	ID = FGuid::NewGuid();
}

bool FBasicProxy::Active()
{
	return true;
}

void FBasicProxy::Cancel()
{

}

void FBasicProxy::Allocation()
{

}

void FBasicProxy::UnAllocation()
{

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

ACharacterBase* FBasicProxy::GetProxyCharacter() const
{
	return OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr.Get();
}

void FBasicProxy::Update2Client()
{
	OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetHoldingItemsComponent()->Proxy_Container.UpdateItem(GetThisSPtr());
}

TSharedPtr<FBasicProxy> FBasicProxy::GetThisSPtr() const
{
	return
		OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetHoldingItemsComponent()->FindProxy(ID);
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

#if UE_EDITOR || UE_CLIENT
	auto ProxyCharacterPtr = OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr;
	if (GetProxyCharacter()->GetNetMode() == NM_Client)
	{
		auto HoldingItemsComponentPtr = ProxyCharacterPtr->GetHoldingItemsComponent();
		if (AllocationCharacterUnitPtr.IsValid())
		{
			HoldingItemsComponentPtr->SetAllocationCharacterUnit(this->GetID(), AllocationCharacterUnitPtr.Pin()->GetID());
		}
		else
		{
			HoldingItemsComponentPtr->SetAllocationCharacterUnit(this->GetID(), FGuid());
		}
	}
#endif

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

bool FConsumableProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Num;

	return true;
}

bool FConsumableProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetPtr =
			new FGameplayAbilityTargetData_Consumable;

		// Test
		GameplayAbilityTargetPtr->ProxyID = GetID();

		const auto InputID = FMath::Rand32();
		FGameplayAbilitySpec AbilitySpec(
			USkill_Consumable_Generic::StaticClass(),
			1,
			InputID
		);

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetPtr);

		auto AllocationCharacter = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;
		AllocationCharacter->GetAbilitySystemComponent()->ReplicateEventData(
			InputID,
			*GameplayEventData
		);

		auto ASCPtr = ProxyCharacterPtr->GetAbilitySystemComponent();
		ASCPtr->GiveAbilityAndActivateOnce(
			AbilitySpec
		);
	}
#endif

	return true;
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
	auto CDSPtr = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetCDCaculatorComponent()->GetCooldown(
		this
	);

	if (CDSPtr)
	{
		return CDSPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
	}

	return true;
}

bool FConsumableProxy::CheckCooldown() const
{
	auto CDSPtr = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetCDCaculatorComponent()->GetCooldown(
		this
	);

	if (CDSPtr)
	{
		return CDSPtr->CheckCooldown();
	}

	return true;
}

void FConsumableProxy::AddCooldownConsumeTime(float NewTime)
{
}

void FConsumableProxy::FreshUniqueCooldownTime()
{
}

void FConsumableProxy::ApplyCooldown()
{
	AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetCDCaculatorComponent()->ApplyCooldown(
		this
	);
}

void FConsumableProxy::OffsetCooldownTime()
{
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

bool FWeaponProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << ActivedWeaponPtr;

	if (Ar.IsSaving())
	{
		bool bIsValid = FirstSkill.IsValid();
		Ar << bIsValid;
		if (bIsValid)
		{
			FirstSkill->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		bool bIsValid = false;
		Ar << bIsValid;
		if (bIsValid)
		{
			auto TempPtr = MakeShared<FWeaponSkillProxy>();
			TempPtr->NetSerialize(Ar, Map, bOutSuccess);

			FirstSkill = TempPtr;
		}
	}

	return true;
}

void FWeaponProxy::InitialUnit()
{
	Super::InitialUnit();
	{
		MaxAttackDistance = GetTableRowUnit_WeaponExtendInfo()->MaxAttackDistance;
	}
}

bool FWeaponProxy::Active()
{
	Super::Active();

	return FirstSkill->Active();
}

void FWeaponProxy::Cancel()
{
	Super::Cancel();

	FirstSkill->Cancel();
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

void FWeaponProxy::Allocation()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		FirstSkill->Allocation();
	}
#endif
}

void FWeaponProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		FirstSkill->UnAllocation();
	}
#endif
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

bool FSkillProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Level;
	Ar << GameplayAbilitySpecHandle;

	return true;
}

TSubclassOf<USkill_Base> FSkillProxy::GetSkillClass() const
{
	return nullptr;
}

void FSkillProxy::Allocation()
{
	RegisterSkill();
}

void FSkillProxy::UnAllocation()
{
	UnRegisterSkill();
}

bool FActiveSkillProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
	 	auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
	 	if (!InGAInsPtr)
	 	{
	 		return false;
	 	}
	 
	 	auto ASCPtr = OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetAbilitySystemComponent();
	 
	 	// 需要特殊参数的
	 	if (
	 		GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active_Control)
	 		)
	 	{
	 		if (InGAInsPtr->IsActive())
	 		{
	 			InGAInsPtr->SetContinuePerform(true);
	 			return true;
	 		}
	 		else
	 		{
	 			auto GameplayAbilityTargetPtr =
	 				new FGameplayAbilityTargetData_Control;
	 
	 			// Test
	 			GameplayAbilityTargetPtr->TargetCharacterPtr = OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr.Get();
	 
	 			FGameplayEventData Payload;
	 			Payload.TargetData.Add(GameplayAbilityTargetPtr);
	 
	 			return ASCPtr->TriggerAbilityFromGameplayEvent(
	 				InGAInsPtr->GetCurrentAbilitySpecHandle(),
	 				ASCPtr->AbilityActorInfo.Get(),
					GetUnitType(),
	 				&Payload,
	 				*ASCPtr
	 			);
	 		}
	 	}
	 	else
	 	{
	 		if (InGAInsPtr->IsActive())
	 		{
	 			InGAInsPtr->SetContinuePerform(true);
	 			return true;
	 		}
	 		else
	 		{
	 			auto GameplayAbilityTargetPtr =
	 				new FGameplayAbilityTargetData_ActiveSkill;
	 
	 			FGameplayEventData Payload;
	 			Payload.TargetData.Add(GameplayAbilityTargetPtr);
	 
	 			return ASCPtr->TriggerAbilityFromGameplayEvent(
	 				InGAInsPtr->GetCurrentAbilitySpecHandle(),
	 				ASCPtr->AbilityActorInfo.Get(),
					GetUnitType(),
	 				&Payload,
	 				*ASCPtr
	 			);
	 		}
	 	}
	}
#endif

	return true;
}

void FActiveSkillProxy::Cancel()
{
}

void FWeaponSkillProxy::SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	Super::SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);
}

bool FWeaponSkillProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto InGaInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
		if (!InGaInsPtr)
		{
			return false;
		}

		if (InGaInsPtr->IsActive())
		{
			InGaInsPtr->SetContinuePerform(true);
			return true;
		}

		FGameplayEventData Payload;
		if (
			GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_Axe) ||
			GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_HandProtection) ||
			GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_RangeTest)
			)
		{
			auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Skill_Weapon;
			GameplayAbilityTargetDashPtr->WeaponPtr = ActivedWeaponPtr;
			GameplayAbilityTargetDashPtr->bIsAutoContinue = true;
			Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
		}

		auto ASCPtr = GetProxyCharacter()->GetAbilitySystemComponent();

		return ASCPtr->TriggerAbilityFromGameplayEvent(
			InGaInsPtr->GetCurrentAbilitySpecHandle(),
			ASCPtr->AbilityActorInfo.Get(),
			GetUnitType(),
			&Payload,
			*ASCPtr
		);
	}
#endif

	return true;
}

void FWeaponSkillProxy::Cancel()
{
	auto InGAInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return;
	}

	InGAInsPtr->SetContinuePerform(false);
}

void FSkillProxy::RegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_SkillBase;

		GameplayAbilityTargetDataPtr->ProxyID = GetID();

		const auto InputID = FMath::RandHelper(std::numeric_limits<int32>::max());
		FGameplayAbilitySpec GameplayAbilitySpec(
			GetSkillClass(),
			Level,
			InputID
		);

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetDataPtr);

		auto AllocationCharacter = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;

		AllocationCharacter->GetAbilitySystemComponent()->ReplicateEventData(
			InputID,
			*GameplayEventData
		);
		GameplayAbilitySpecHandle = AllocationCharacter->GetAbilitySystemComponent()->GiveAbility(GameplayAbilitySpec);
	}
#endif
}

void FSkillProxy::UnRegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (GetAllocationCharacterUnit().IsValid())
		{
			auto AllocationCharacter = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;

			if (AllocationCharacter.IsValid())
			{
				auto ASCPtr = AllocationCharacter->GetAbilitySystemComponent();

				ASCPtr->CancelAbilityHandle(GameplayAbilitySpecHandle);
				ASCPtr->ClearAbility(GameplayAbilitySpecHandle);
			}
		}
	}
#endif

	GameplayAbilitySpecHandle = FGameplayAbilitySpecHandle();
}

TArray<USkill_Base*> FSkillProxy::GetGAInstAry()const
{
	TArray<USkill_Base*>ResultAry;
	auto ProxyCharacterPtr = GetProxyCharacter();
	auto ASCPtr = ProxyCharacterPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
	if (GameplayAbilitySpecPtr)
	{
		ResultAry.Add(Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance()));
	}

	return ResultAry;
}

USkill_Base* FSkillProxy::GetGAInst()const
{
	auto ProxyCharacterPtr = GetProxyCharacter();
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

bool FWeaponSkillProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << ActivedWeaponPtr;

	return true;
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
}

bool FCharacterProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << ProxyCharacterPtr;

	return true;
}

void FCharacterProxy::InitialUnit()
{
	Super::InitialUnit();

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
	auto CDSPtr = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetCDCaculatorComponent()->GetCooldown(
		this
	);

	if (CDSPtr)
	{
		return CDSPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
	}

	return true;
}

bool FActiveSkillProxy::CheckCooldown() const
{
	auto CDSPtr = AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetCDCaculatorComponent()->GetCooldown(
		this
	);

	if (CDSPtr)
	{
		return CDSPtr->CheckCooldown();
	}

	return true;
}

void FActiveSkillProxy::AddCooldownConsumeTime(float NewTime)
{
}

void FActiveSkillProxy::FreshUniqueCooldownTime()
{
}

void FActiveSkillProxy::ApplyCooldown()
{
	AllocationCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetCDCaculatorComponent()->ApplyCooldown(
		this
	);
}

void FActiveSkillProxy::OffsetCooldownTime()
{
}

void FWeaponProxy::ActiveWeapon()
{
	if (ActivedWeaponPtr)
	{
		check(0);
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		auto ProxyCharacterPtr = GetProxyCharacter();
		if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			auto TableRowUnit_WeaponExtendInfoPtr = GetTableRowUnit_WeaponExtendInfo();

			ProxyCharacterPtr->SwitchAnimLink_Client(TableRowUnit_WeaponExtendInfoPtr->AnimLinkClassType);

			auto ToolActorClass = TableRowUnit_WeaponExtendInfoPtr->ToolActorClass;

			FActorSpawnParameters SpawnParameters;

			auto AllocationCharacter = GetAllocationCharacterUnit().Pin()->ProxyCharacterPtr;

			SpawnParameters.Owner = AllocationCharacter.Get();

			ActivedWeaponPtr = GWorld->SpawnActor<AWeapon_Base>(ToolActorClass, SpawnParameters);
			ActivedWeaponPtr->SetWeaponUnit(*this);

			FirstSkill->ActivedWeaponPtr = ActivedWeaponPtr;
		}
#endif
	}
}

void FWeaponProxy::RetractputWeapon()
{
	if (ActivedWeaponPtr)
	{
		ActivedWeaponPtr->Destroy();
		ActivedWeaponPtr = nullptr;

		FirstSkill->Cancel();
		FirstSkill->ActivedWeaponPtr = nullptr;
	}

#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr.Get();
		OnwerActorPtr->SwitchAnimLink_Client(EAnimLinkClassType::kUnarmed);
	}
#endif
}
