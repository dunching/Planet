
#include "ItemProxy_Minimal.h"

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

ACharacterBase* FBasicProxy::GetOwnerCharacter() const
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
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (GetOwnerCharacter()->GetNetMode() == NM_Client)
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
	auto ProxyCharacterPtr = GetOwnerCharacter();
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
