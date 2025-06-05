#include "ItemProxy_Weapon.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "DataTableCollection.h"
#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "PropertyEntrys.h"
#include "Weapon_Base.h"
#include "InventoryComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"
#include "ItemProxy_Skills.h"
#include "ModifyItemProxyStrategy.h"
#include "PlanetWeapon_Base.h"

TSharedPtr<FWeaponSkillProxy> FWeaponProxy::GetWeaponSkill()
{
	return DynamicCastSharedPtr<FWeaponSkillProxy>(GetInventoryComponent()->FindProxy<FModifyItemProxyStrategy_WeaponSkill>(WeaponSkillID));
}

FWeaponProxy::FWeaponProxy()
{
}

void FWeaponProxy::UpdateByRemote(
	const TSharedPtr<FWeaponProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);
	UpdateByRemote_Allocationble(RemoteSPtr);

	MaxAttackDistance = RemoteSPtr->MaxAttackDistance;
	ActivedWeaponPtr = RemoteSPtr->ActivedWeaponPtr;
	WeaponSkillID = RemoteSPtr->WeaponSkillID;
}

bool FWeaponProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Allocationble(Ar, Map, bOutSuccess);

	Ar << MaxAttackDistance;
	Ar << ActivedWeaponPtr;
	Ar << WeaponSkillID;

	return true;
}

void FWeaponProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);

	ProxyPtr = this;
	{
		MaxAttackDistance = GetTableRowProxy_WeaponExtendInfo()->MaxAttackDistance;
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

void FWeaponProxy::SetAllocationCharacterProxy(
	const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
	const FGameplayTag& InSocketTag
	)
{
	IProxy_Allocationble::SetAllocationCharacterProxy(InAllocationCharacterProxyPtr, InSocketTag);

#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
	}
#endif

	GetWeaponSkill()->SetAllocationCharacterProxy(InAllocationCharacterProxyPtr, InSocketTag);
}

UItemProxy_Description_Weapon* FWeaponProxy::GetTableRowProxy_WeaponExtendInfo() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_WeaponPtr = Cast<UItemProxy_Description_Weapon>(
	                                                                           TableRowPtr->ItemProxy_Description.
	                                                                           LoadSynchronous()
	                                                                          );
	return ItemProxy_Description_WeaponPtr;
}

void FWeaponProxy::Allocation()
{
	Super::Allocation();

#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		GetWeaponSkill()->Allocation();
	}
#endif
}

void FWeaponProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		GetWeaponSkill()->UnAllocation();
	}
#endif
	Super::UnAllocation();

	RetractputWeapon();
}

FTableRowProxy_PropertyEntrys* FWeaponProxy::GetMainPropertyEntry() const
{
	auto SceneProxyExtendInfoMapPtr = UDataTableCollection::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_PropertyEntrys.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_PropertyEntrys>(
		 *GetTableRowProxy_WeaponExtendInfo()->PropertyEntry.ToString(),
		 TEXT("GetProxy")
		);
	return SceneProxyExtendInfoPtr;
}

int32 FWeaponProxy::GetMaxAttackDistance() const
{
	return MaxAttackDistance;
}

void FWeaponProxy::ActiveWeapon()
{
	if (ActivedWeaponPtr)
	{
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
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
			}

			// 切换人物姿势
			auto TableRowProxy_WeaponExtendInfoPtr = GetTableRowProxy_WeaponExtendInfo();

			// 生成对应的武器Actor
			auto ToolActorClass = TableRowProxy_WeaponExtendInfoPtr->ToolActorClass;

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetAllocationCharacter();

			auto AllocationCharacter = GetAllocationCharacterProxy()->GetCharacterActor();

			ActivedWeaponPtr = GWorld->SpawnActor<APlanetWeapon_Base>(ToolActorClass, SpawnParameters);

			GetWeaponSkill()->ActivedWeaponPtr = ActivedWeaponPtr;
		}
#endif
	}

#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		if (ActivedWeaponPtr)
		{
			ActivedWeaponPtr->SetWeaponProxy(GetID());
		}

		// 切换人物姿势
		auto TableRowProxy_WeaponExtendInfoPtr = GetTableRowProxy_WeaponExtendInfo();

		auto AllocationCharacterPtr = GetAllocationCharacter();
		AllocationCharacterPtr->SwitchAnimLink_Client(TableRowProxy_WeaponExtendInfoPtr->AnimLinkClassType);
	}
#endif
}

void FWeaponProxy::RetractputWeapon()
{
	if (ActivedWeaponPtr)
	{
		// 移除武器给的属性词条
#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
		{
			auto AllocationCharacter = GetAllocationCharacter();
			if (AllocationCharacter->GetNetMode() == NM_DedicatedServer)
			{
			}

			ActivedWeaponPtr->Destroy();
		}
#endif
		ActivedWeaponPtr = nullptr;

		GetWeaponSkill()->End();
		GetWeaponSkill()->ActivedWeaponPtr = nullptr;
	}

#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		auto AllocationCharacterPtr = GetAllocationCharacter();
		if (AllocationCharacterPtr)
		{
			AllocationCharacterPtr->SwitchAnimLink_Client(EAnimLinkClassType::kUnarmed);
		}
	}
#endif
}
