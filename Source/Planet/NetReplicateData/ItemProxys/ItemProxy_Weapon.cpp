
#include "ItemProxy_Weapon.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "SceneUnitExtendInfo.h"
#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "PropertyEntrys.h"
#include "Weapon_Base.h"
#include "HoldingItemsComponent.h"
#include "BaseFeatureComponent.h"
#include "ItemProxy_Minimal.h"

TSharedPtr<FWeaponSkillProxy> FWeaponProxy::GetWeaponSkill()
{
	return DynamicCastSharedPtr<FWeaponSkillProxy>(HoldingItemsComponentPtr->FindUnit_Skill(WeaponSkillID));
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
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
//		GetWeaponSkill()->Allocation();
	}
#endif
}

void FWeaponProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
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

void FWeaponProxy::ActiveWeapon()
{
	if (ActivedWeaponPtr)
	{
		checkNoEntry();
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		auto ProxyCharacterPtr = GetOwnerCharacter();
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
			ActivedWeaponPtr->SetWeaponProxy(GetID());

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
		auto ProxyCharacterPtr = GetOwnerCharacter();
		if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			auto AllocationCharacter = GetAllocationCharacter();
			if (AllocationCharacter->GetNetMode() == NM_DedicatedServer)
			{
				AllocationCharacter->GetBaseFeatureComponent()->ClearData2Self(GetAllData(), GetUnitType());
			}
		}
#endif

		ActivedWeaponPtr->Destroy();
		ActivedWeaponPtr = nullptr;

		GetWeaponSkill()->End();
		GetWeaponSkill()->ActivedWeaponPtr = nullptr;
	}

#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto AllocationCharacterPtr = GetAllocationCharacter();
		AllocationCharacterPtr->SwitchAnimLink_Client(EAnimLinkClassType::kUnarmed);
	}
#endif
}
