
#include "AllocationSkills.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitContainer.h"
#include "HoldingItemsComponent.h"

bool FAllocation_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FAllocation_FASI_Container::AddItem(const TSharedPtr<FItemType>& ProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (HoldingItemsComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			FItemType Item;

			Item = *ProxySPtr;

			auto& Ref = Items.Add_GetRef(Item);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FAllocation_FASI_Container::UpdateItem(const TSharedPtr<FItemType>& ProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (HoldingItemsComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].Socket == ProxySPtr->Socket)
				{
					Items[Index] = *ProxySPtr;

					MarkItemDirty(Items[Index]);
					return;
				}
			}
		}
	}
#endif
}

void FSocket_FASI::PreReplicatedRemove(const FAllocation_FASI_Container& InArraySerializer)
{

}

void FSocket_FASI::PostReplicatedAdd(const FAllocation_FASI_Container& InArraySerializer)
{
	PostReplicatedChange(InArraySerializer);
}

void FSocket_FASI::PostReplicatedChange(const FAllocation_FASI_Container& InArraySerializer)
{
	// 在这里 我们对本地的数据进行绑定

	TSharedPtr<FSocket_FASI> SPtr = MakeShared<FSocket_FASI>();

	if (ProxySPtr)
	{
		const auto UnitType = ProxySPtr->GetUnitType();

		if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
		{
			auto TempProxySPtr = InArraySerializer.HoldingItemsComponentPtr->FindUnit_Weapon(ProxySPtr->GetID());

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
		{
			auto TempProxySPtr = InArraySerializer.HoldingItemsComponentPtr->FindUnit_Skill(ProxySPtr->GetID());

			ProxySPtr = TempProxySPtr;
		}
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
		{
			auto TempProxySPtr = InArraySerializer.HoldingItemsComponentPtr->FindUnit_Skill(ProxySPtr->GetID());

			ProxySPtr = TempProxySPtr;
		}

		*SPtr = *this;

		InArraySerializer.UnitProxyProcessComponentPtr->Update(SPtr);
	}
	else
	{
		*SPtr = *this;

		InArraySerializer.UnitProxyProcessComponentPtr->Update(SPtr);
	}
}

bool FSocket_FASI::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto KeyStr = Key.ToString();
		Ar << KeyStr;

		Ar << Socket;

		if (ProxySPtr)
		{
			auto UnitType = ProxySPtr->GetUnitType();
			Ar << UnitType;

			if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
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
			else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
			{
				auto TempProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
				TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
		}
		else
		{
			auto UnitType = FGameplayTag::EmptyTag;
			Ar << UnitType;
		}
	}
	else if (Ar.IsLoading())
	{
		FString KeyStr;
		Ar << KeyStr;
		Key = FKey(*KeyStr);

		Ar << Socket;

		FGameplayTag UnitType = FGameplayTag::EmptyTag;
		Ar << UnitType;

		if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
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
		else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
		{
			auto TempProxySPtr = MakeShared<FConsumableProxy>();
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);

			ProxySPtr = TempProxySPtr;
		}
	}

	return true;
}
