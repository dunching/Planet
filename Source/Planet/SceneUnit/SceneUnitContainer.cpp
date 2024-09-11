
#include "SceneUnitContainer.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "HoldingItemsComponent.h"
#include "GameplayTagsSubSystem.h"
#include "SceneElement.h"
#include "TemplateHelper.h"

FTableRowUnit* GetTableRowUnit(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoMapPtr = USceneUnitExtendInfoMap::GetInstance();
	auto DataTable = SceneUnitExtendInfoMapPtr->DataTable_Unit.LoadSynchronous();

	auto SceneUnitExtendInfoPtr = DataTable->FindRow<FTableRowUnit>(*UnitType.ToString(), TEXT("GetUnit"));

	return SceneUnitExtendInfoPtr;
}

bool FProxy_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FProxy_FASI_Container::AddItem(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (HoldingItemsComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			FProxy_FASI Proxy_FASI;

			Proxy_FASI.ProxySPtr = ProxySPtr;

			auto& Ref = Items.Add_GetRef(Proxy_FASI);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FProxy_FASI_Container::UpdateItem(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (HoldingItemsComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].ProxySPtr == ProxySPtr)
				{
					MarkItemDirty(Items[Index]);
					return;
				}
			}
		}
	}
#endif
}

void FProxy_FASI_Container::RemoveItem(const FItemType& Item)
{
	for (int32 Idx = 0; Idx < Items.Num(); ++Idx)
	{
		if (Items[Idx] == Item)
		{
			Items.RemoveAt(Idx);

			MarkArrayDirty();
			return;
		}
	}
}

TSharedPtr<FBasicProxy> FProxy_FASI_Container::GetProxyType(const FGameplayTag& UnitType)
{
	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
		return MakeShared<FToolProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		return  MakeShared<FWeaponProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		return  MakeShared<FActiveSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
	{
		return  MakeShared<FPassiveSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent))
	{
		return MakeShared<FTalentSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
	{
		return MakeShared<FWeaponSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->DataSource_Character))
	{
	}

	return nullptr;
}

void FProxy_FASI::PreReplicatedRemove(const struct FProxy_FASI_Container& InArraySerializer)
{

}

void FProxy_FASI::PostReplicatedAdd(const struct FProxy_FASI_Container& InArraySerializer)
{
	const auto UnitType = ProxySPtr->GetUnitType();

	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{

	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		InArraySerializer.HoldingItemsComponentPtr->Update_Weapon(DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr));
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		InArraySerializer.HoldingItemsComponentPtr->Update_Skill(DynamicCastSharedPtr<FActiveSkillProxy>(ProxySPtr));
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
	{
		InArraySerializer.HoldingItemsComponentPtr->Update_Skill(DynamicCastSharedPtr<FPassiveSkillProxy>(ProxySPtr));
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent))
	{
		InArraySerializer.HoldingItemsComponentPtr->Update_Skill(DynamicCastSharedPtr<FTalentSkillProxy>(ProxySPtr));
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
	{
		InArraySerializer.HoldingItemsComponentPtr->Update_Skill(DynamicCastSharedPtr<FWeaponSkillProxy>(ProxySPtr));
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate_Player))
	{
		// 1.使用远程的数据
		*InArraySerializer.HoldingItemsComponentPtr->CharacterProxySPtr = *DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);

		// 2.
		InArraySerializer.HoldingItemsComponentPtr->Update_Character(
			InArraySerializer.HoldingItemsComponentPtr->CharacterProxySPtr
		);

		// 3.替换 Caontainer 下的副本
		ProxySPtr = InArraySerializer.HoldingItemsComponentPtr->CharacterProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
	{
		InArraySerializer.HoldingItemsComponentPtr->Update_Character(DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr));
	}
}

bool FProxy_FASI::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto UnitType = ProxySPtr->GetUnitType();
		Ar << UnitType;

		if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FToolProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		FGameplayTag UnitType = FGameplayTag::EmptyTag;
		Ar << UnitType;

		if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
		{
			auto TempProxySPtr = MakeShared<FToolProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
		{
			auto TempProxySPtr = MakeShared<FWeaponProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
		{
			auto TempProxySPtr = MakeShared<FActiveSkillProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
		{
			auto TempProxySPtr = MakeShared<FPassiveSkillProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent))
		{
			auto TempProxySPtr = MakeShared<FTalentSkillProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
		{
			auto TempProxySPtr = MakeShared<FWeaponSkillProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
		{
			auto TempProxySPtr = MakeShared<FCoinProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
		{
			auto TempProxySPtr = MakeShared<FConsumableProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
		{
			auto TempProxySPtr = MakeShared<FCharacterProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
	}

	return true;
}

bool FProxy_FASI::operator==(const FProxy_FASI& Right) const
{
	if (ProxySPtr && Right.ProxySPtr)
	{
		return ProxySPtr->GetUnitType() == Right.ProxySPtr->GetUnitType();
	}

	return true;
}
