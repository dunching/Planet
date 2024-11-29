
#include "ItemProxy.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "GroupMnaggerComponent.h"
#include "PropertyEntrys.h"
#include "CharactersInfo.h"
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
#include "BaseFeatureComponent.h"
#include "Weapon_Bow.h"
#include "Skill_WeaponActive_Bow.h"
#include "Skill_WeaponActive_FoldingFan.h"
#include "ItemProxy_Character.h"

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
	Ar << OwnerCharacter_ID;
	Ar << AllocationCharacter_ID;

	return true;
}

void FBasicProxy::InitialUnit()
{
	if (!ID.IsValid())
	{
		ID = FGuid::NewGuid();
	}
}

void FBasicProxy::UpdateByRemote(const TSharedPtr<FBasicProxy>& RemoteSPtr)
{
	ID = RemoteSPtr->ID;
	OwnerCharacter_ID = RemoteSPtr->OwnerCharacter_ID;
	AllocationCharacter_ID = RemoteSPtr->AllocationCharacter_ID;
	UnitType = RemoteSPtr->UnitType;
}

bool FBasicProxy::CanActive() const
{
	return true;
}

bool FBasicProxy::Active()
{
	return true;
}

void FBasicProxy::Cancel()
{

}

void FBasicProxy::End()
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
	return HoldingItemsComponentPtr->FindProxy_Character(OwnerCharacter_ID)->ProxyCharacterPtr.Get();
}

ACharacterBase* FBasicProxy::GetAllocationCharacter() const
{
	return HoldingItemsComponentPtr->FindProxy_Character(AllocationCharacter_ID)->ProxyCharacterPtr.Get();
}

void FBasicProxy::Update2Client()
{
	HoldingItemsComponentPtr->Proxy_Container.UpdateItem(GetID());
}

TWeakPtr<FCharacterProxy> FBasicProxy::GetOwnerCharacterProxy()const
{
	return HoldingItemsComponentPtr->FindProxy_Character(OwnerCharacter_ID);
}

TWeakPtr<FCharacterProxy> FBasicProxy::GetAllocationCharacterProxy()
{
	return HoldingItemsComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

FString FBasicProxy::GetUnitName() const
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit();

	return SceneUnitExtendInfoPtr->UnitName;
}

void FBasicProxy::SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	if (InAllocationCharacterUnitPtr)
	{
		if (AllocationCharacter_ID == InAllocationCharacterUnitPtr->GetID())
		{
			return;
		}

		AllocationCharacter_ID = InAllocationCharacterUnitPtr->GetID();
	}
	else
	{
		AllocationCharacter_ID = FGuid();
	}

#if UE_EDITOR || UE_CLIENT
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (GetProxyCharacter()->GetNetMode() == NM_Client)
	{
		HoldingItemsComponentPtr->SetAllocationCharacterUnit(this->GetID(), AllocationCharacter_ID);
	}
#endif

	OnAllocationCharacterUnitChanged.ExcuteCallback(GetAllocationCharacterProxy());
}

TWeakPtr<FCharacterProxy> FBasicProxy::GetAllocationCharacterProxy() const
{
	return HoldingItemsComponentPtr->FindProxy_Character(AllocationCharacter_ID);
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

void FConsumableProxy::UpdateByRemote(const TSharedPtr<FConsumableProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	const auto OldValue = Num;

	Num = RemoteSPtr->Num;

	CallbackContainerHelper.ValueChanged(OldValue, Num);
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

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;
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
	auto CDSPtr = GetAllocationCharacter()->GetCDCaculatorComponent()->GetCooldown(
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
	auto CDSPtr = GetAllocationCharacter()->GetCDCaculatorComponent()->GetCooldown(
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
	GetAllocationCharacter()->GetCDCaculatorComponent()->ApplyCooldown(
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

void FWeaponProxy::UpdateByRemote(const TSharedPtr<FWeaponProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	MaxAttackDistance = RemoteSPtr->MaxAttackDistance;
}

bool FWeaponProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << ActivedWeaponPtr;
	Ar << WeaponSkillID;

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

	return GetWeaponSkill()->Active();
}

void FWeaponProxy::Cancel()
{
	Super::Cancel();

	GetWeaponSkill()->Cancel();
}

void FWeaponProxy::SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	Super::SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);

	GetWeaponSkill()->SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);
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
	Super::Allocation();
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
//		GetWeaponSkill()->Allocation();
	}
#endif
}

void FWeaponProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
//		GetWeaponSkill()->UnAllocation();
	}
#endif
	Super::UnAllocation();
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

TSharedPtr<FWeaponSkillProxy> FWeaponProxy::GetWeaponSkill()
{
	return DynamicCastSharedPtr<FWeaponSkillProxy>(HoldingItemsComponentPtr->FindUnit_Skill(WeaponSkillID));
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

void FSkillProxy::SetAllocationCharacterUnit(const TSharedPtr<FCharacterProxy>& InAllocationCharacterUnitPtr)
{
	if (!InAllocationCharacterUnitPtr)
	{
		UnAllocation();
	}

	Super::SetAllocationCharacterUnit(InAllocationCharacterUnitPtr);
}

void FSkillProxy::UpdateByRemote(const TSharedPtr<FSkillProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	Level = RemoteSPtr->Level;
	GameplayAbilitySpecHandle = RemoteSPtr->GameplayAbilitySpecHandle;
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
			GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Skill_Weapon) 
			)
		{
			auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_WeaponActive_ActiveParam;
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

void FWeaponSkillProxy::End()
{
	auto InGAInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return;
	}

	InGAInsPtr->K2_CancelAbility();
}

void FSkillProxy::RegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_SkillBase_RegisterParam;

		GameplayAbilityTargetDataPtr->ProxyID = GetID();

		const auto InputID = FMath::RandHelper(std::numeric_limits<int32>::max());
		FGameplayAbilitySpec GameplayAbilitySpec(
			GetSkillClass(),
			Level,
			InputID
		);

		FGameplayEventData GameplayEventData;
		GameplayEventData.TargetData.Add(GameplayAbilityTargetDataPtr);

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

		AllocationCharacter->GetAbilitySystemComponent()->ReplicateEventData(
			InputID,
			GameplayEventData
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
		if (GetAllocationCharacterProxy().IsValid())
		{
			auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

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

void FWeaponSkillProxy::RegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		FGameplayAbilityTargetData_SkillBase_RegisterParam* GameplayAbilityTargetDataPtr = nullptr;
		// 需要特殊参数的
		if (
			GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Skill_Weapon_Bow)
			)
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Bow_RegisterParam;
		}
		else if (
			GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Skill_Weapon_FoldingFan)
			)
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_FoldingFan_RegisterParam;
		}
		else
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_SkillBase_RegisterParam;
		}
		GameplayAbilityTargetDataPtr->ProxyID = GetID();

		const auto InputID = FMath::RandHelper(std::numeric_limits<int32>::max());
		FGameplayAbilitySpec GameplayAbilitySpec(
			GetSkillClass(),
			Level,
			InputID
		);

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetDataPtr);

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

		AllocationCharacter->GetAbilitySystemComponent()->ReplicateEventData(
			InputID,
			*GameplayEventData
		);
		GameplayAbilitySpecHandle = AllocationCharacter->GetAbilitySystemComponent()->GiveAbility(GameplayAbilitySpec);
	}
#endif
}

FTalentSkillProxy::FTalentSkillProxy()
{
}

FActiveSkillProxy::FActiveSkillProxy()
{
}

bool FActiveSkillProxy::CanActive() const
{
	auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return false;
	}

	// 本地判断是否能释放（有些条件仅本地存在，比如是否锁定了目标）
	if (
		InGAInsPtr->CanActivateAbility(InGAInsPtr->GetCurrentAbilitySpecHandle(),
			InGAInsPtr->GetCurrentActorInfo())
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FActiveSkillProxy::Active()
{
	// #if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	// 	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
		if (!InGAInsPtr)
		{
			return false;
		}

		auto ASCPtr = GetProxyCharacter()->GetAbilitySystemComponent();

		// 需要特殊参数的
		if (
			GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Skill_Active_Control)
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
				GameplayAbilityTargetPtr->TargetCharacterPtr = GetAllocationCharacter();

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
					new FGameplayAbilityTargetData_ActiveSkill_ActiveParam;

				GameplayAbilityTargetPtr->bIsAutoContinue =
					ProxyCharacterPtr->GetRemoteRole() == ROLE_AutonomousProxy ? false : true;

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
	//#endif

	//	return true;
}

void FActiveSkillProxy::Cancel()
{
}

bool FActiveSkillProxy::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	auto CDSPtr = GetProxyCharacter()->GetCDCaculatorComponent()->GetCooldown(
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
	auto CDSPtr = GetProxyCharacter()->GetCDCaculatorComponent()->GetCooldown(
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
	GetProxyCharacter()->GetCDCaculatorComponent()->ApplyCooldown(
		this
	);
}

void FActiveSkillProxy::OffsetCooldownTime()
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
	Super::InitialUnit();
}

void FPassiveSkillProxy::Allocation()
{
	Super::Allocation();

#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto AllocationCharacter = GetAllocationCharacter();
		// 词条
		{
			auto MainPropertyEntryPtr = GetMainPropertyEntry();
			if (MainPropertyEntryPtr)
			{
				TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

				for (const auto& Iter : MainPropertyEntryPtr->Map)
				{
					ModifyPropertyMap.Add(Iter);
				}

				AllocationCharacter->GetBaseFeatureComponent()->SendEvent2Self(
					ModifyPropertyMap, GetUnitType()
				);
			}
		}
	}
#endif
}

void FPassiveSkillProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto AllocationCharacter = GetAllocationCharacter();
		AllocationCharacter->GetBaseFeatureComponent()->ClearData2Self(
			GetAllData(), GetUnitType()
		);
	}
#endif

	Super::UnAllocation();
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

FCoinProxy::FCoinProxy()
{

}

void FCoinProxy::UpdateByRemote(const TSharedPtr<FCoinProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	Num = RemoteSPtr->Num;
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

void FWeaponProxy::ActiveWeapon()
{
	if (ActivedWeaponPtr)
	{
		checkNoEntry();
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		auto ProxyCharacterPtr = GetProxyCharacter();
		if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			// 添加武器给的属性词条
			auto PropertyEntrysPtr = GetMainPropertyEntry();
			if (PropertyEntrysPtr)
			{
				TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

				for (const auto& Iter : PropertyEntrysPtr->Map)
				{
					ModifyPropertyMap.Add(Iter);
				}

				auto AllocationCharacter = GetAllocationCharacter();
				AllocationCharacter->GetBaseFeatureComponent()->SendEvent2Self(
					ModifyPropertyMap, GetUnitType()
				);
			}

			// 
			auto TableRowUnit_WeaponExtendInfoPtr = GetTableRowUnit_WeaponExtendInfo();

			ProxyCharacterPtr->SwitchAnimLink_Client(TableRowUnit_WeaponExtendInfoPtr->AnimLinkClassType);

			auto ToolActorClass = TableRowUnit_WeaponExtendInfoPtr->ToolActorClass;

			// 生成对应的武器Actor
			FActorSpawnParameters SpawnParameters;

			auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

			SpawnParameters.Owner = ProxyCharacterPtr;

			ActivedWeaponPtr = GWorld->SpawnActor<AWeapon_Base>(ToolActorClass, SpawnParameters);
			ActivedWeaponPtr->SetWeaponUnit(GetID());

			GetWeaponSkill()->ActivedWeaponPtr = ActivedWeaponPtr;
		}
#endif
	}
}

void FWeaponProxy::RetractputWeapon()
{
	if (ActivedWeaponPtr)
	{
		// 移除武器给的属性词条
#if UE_EDITOR || UE_SERVER
		auto AllocationCharacter = GetAllocationCharacter();
		if (AllocationCharacter->GetNetMode() == NM_DedicatedServer)
		{
			AllocationCharacter->GetBaseFeatureComponent()->ClearData2Self(GetAllData(), GetUnitType());
		}
#endif

		ActivedWeaponPtr->Destroy();
		ActivedWeaponPtr = nullptr;

		GetWeaponSkill()->End();
		GetWeaponSkill()->ActivedWeaponPtr = nullptr;
	}

#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetProxyCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto OnwerActorPtr = GetAllocationCharacter();
		OnwerActorPtr->SwitchAnimLink_Client(EAnimLinkClassType::kUnarmed);
	}
#endif
}
