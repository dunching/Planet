#include "ItemProxy_Container.h"

#include "AssetRefMap.h"
#include "PlanetModule.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneProxyExtendInfo.h"
#include "InventoryComponent.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Minimal.h"
#include "TemplateHelper.h"
#include "CharactersInfo.h"
#include "ItemProxy_Character.h"

FTableRowProxy* GetTableRowProxy(
	const FGameplayTag& ProxyType
	)
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy>(*ProxyType.ToString(), TEXT("GetProxy"));

	return SceneProxyExtendInfoPtr;
}

void FProxy_FASI::PreReplicatedRemove(
	const struct FProxy_FASI_Container& InArraySerializer
	)
{
	// 在这里 我们对本地的数据进行绑定

	InArraySerializer.InventoryComponentPtr->RemoveProxy_SyncHelper(ProxySPtr);
}

void FProxy_FASI::PostReplicatedAdd(
	const struct FProxy_FASI_Container& InArraySerializer
	)
{
	// 在这里 我们对本地的数据进行绑定

	ProxySPtr = InArraySerializer.InventoryComponentPtr->AddProxy_SyncHelper(CacheProxySPtr);
}

void FProxy_FASI::PostReplicatedChange(
	const struct FProxy_FASI_Container& InArraySerializer
	)
{
	// 在这里 我们对本地的数据进行绑定

	InArraySerializer.InventoryComponentPtr->UpdateProxy_SyncHelper(ProxySPtr, CacheProxySPtr);
}

bool FProxy_FASI::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	if (Ar.IsSaving())
	{
		auto ProxyType = ProxySPtr->GetProxyType();
		Ar << ProxyType;

		if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
		{
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
		{
			auto TempProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
			TempProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		FGameplayTag ProxyType = FGameplayTag::EmptyTag;
		Ar << ProxyType;

		if (CacheProxySPtr)
		{
			CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
		}
		else
		{
			if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
			{
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
			{
				CacheProxySPtr = MakeShared<FWeaponProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
			{
				CacheProxySPtr = MakeShared<FActiveSkillProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
			{
				CacheProxySPtr = MakeShared<FPassiveSkillProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
			{
				CacheProxySPtr = MakeShared<FTalentSkillProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
			{
				CacheProxySPtr = MakeShared<FWeaponSkillProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
			{
				CacheProxySPtr = MakeShared<FCoinProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
			{
				CacheProxySPtr = MakeShared<FConsumableProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
			else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
			{
				CacheProxySPtr = MakeShared<FCharacterProxy>();
				CacheProxySPtr->NetSerialize(Ar, Map, bOutSuccess);
			}
		}
	}

	return true;
}

bool FProxy_FASI::operator==(
	const FProxy_FASI& Right
	) const
{
	if (ProxySPtr && Right.ProxySPtr)
	{
		return ProxySPtr->GetProxyType() == Right.ProxySPtr->GetProxyType();
	}

	return true;
}

bool FProxy_FASI_Container::NetDeltaSerialize(
	FNetDeltaSerializeInfo& DeltaParms
	)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FProxy_FASI_Container::AddItem(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (InventoryComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			FItemType Item;

			Item.ProxySPtr = ProxySPtr;

			auto& Ref = Items.Add_GetRef(Item);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FProxy_FASI_Container::UpdateItem(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (InventoryComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].ProxySPtr == ProxySPtr)
				{
					// 注意：ProxySPtr 这个指针已经在外部进行了修改，在这部我们不必再对 Items[Index] 进行修改
					MarkItemDirty(Items[Index]);
					return;
				}
			}

			AddItem(ProxySPtr);
		}
	}
#endif
}

void FProxy_FASI_Container::UpdateItem(
	const FGuid& Proxy_ID
	)
{
#if UE_EDITOR || UE_SERVER
	if (InventoryComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); Index++)
		{
			if (Items[Index].ProxySPtr->GetID() == Proxy_ID)
			{
				// 注意：ProxySPtr 这个指针已经在外部进行了修改，在这部我们不必再对 Items[Index] 进行修改
				MarkItemDirty(Items[Index]);
				return;
			}
		}
	}
#endif
}

void FProxy_FASI_Container::RemoveItem(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (InventoryComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); ++Index)
		{
			if (Items[Index].ProxySPtr == ProxySPtr)
			{
				Items.RemoveAt(Index);

				MarkArrayDirty();
				return;
			}
		}
	}
#endif
}

TSharedPtr<FBasicProxy> FProxy_FASI_Container::GetProxyType(
	const FGameplayTag& ProxyType
	)
{
	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		return MakeShared<FWeaponProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
	{
		return MakeShared<FActiveSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
	{
		return MakeShared<FPassiveSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
	{
		return MakeShared<FTalentSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		return MakeShared<FWeaponSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::DataSource_Character))
	{
	}

	return nullptr;
}
