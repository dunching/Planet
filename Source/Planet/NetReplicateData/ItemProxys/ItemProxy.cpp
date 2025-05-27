#include "ItemProxy.h"

#include "ItemProxy_Minimal.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "PlanetModule.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneProxyExtendInfo.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "TeamMatesHelperComponent.h"
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
#include "InventoryComponent.h"
#include "ConversationComponent.h"
#include "Skill_Consumable_Generic.h"
#include "CharacterAbilitySystemComponent.h"
#include "Weapon_Bow.h"
#include "Skill_WeaponActive_Bow.h"
#include "Skill_WeaponActive_FoldingFan.h"
#include "ItemProxy_Character.h"
#include "ItemProxy_Description.h"

FBasicProxy::FBasicProxy()
{
}

FBasicProxy::~FBasicProxy()
{
}

bool FBasicProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << ProxyType;
	Ar << ID;

	return true;
}

void FBasicProxy::InitialProxy(const FGameplayTag& InProxyType)
{
	ProxyType = InProxyType;
	if (!ID.IsValid())
	{
		ID = FGuid::NewGuid();
	}
}

void FBasicProxy::UpdateByRemote(const TSharedPtr<FBasicProxy>& RemoteSPtr)
{
	GetInventoryComponent()->UpdateID(RemoteSPtr->ID, ID);
	ID = RemoteSPtr->ID;
	
	ProxyType = RemoteSPtr->ProxyType;
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

FBasicProxy::IDType FBasicProxy::GetID() const
{
	return ID;
}

FGameplayTag FBasicProxy::GetProxyType() const
{
	return ProxyType;
}

TSoftObjectPtr<UTexture2D> FBasicProxy::GetIcon() const
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy();

	if (SceneProxyExtendInfoPtr->ItemProxy_Description.ToSoftObjectPath().IsAsset())
	{
		return SceneProxyExtendInfoPtr->ItemProxy_Description.LoadSynchronous()->DefaultIcon;
	}
	return nullptr;
}

void FBasicProxy::Update2Client()
{
#if UE_EDITOR || UE_SERVER
	if (InventoryComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		InventoryComponentPtr->Proxy_Container.UpdateItem(GetID());
	}
#endif
}

TObjectPtr<UItemProxy_Description> FBasicProxy::GetItemProxy_Description() const
{
	auto TableRowPtr = GetTableRowProxy();

	return TableRowPtr->ItemProxy_Description.LoadSynchronous();
}

UInventoryComponent* FBasicProxy::GetInventoryComponent() const
{
	return InventoryComponentPtr;
}

bool FBasicProxy::IsUnique() const
{
	return false;
}

FString FBasicProxy::GetProxyName() const
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy();

	if (SceneProxyExtendInfoPtr->ItemProxy_Description)
	{
		return SceneProxyExtendInfoPtr->ItemProxy_Description.LoadSynchronous()->ProxyName;
	}

	return TEXT("");
}

FTableRowProxy* FBasicProxy::GetTableRowProxy() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag)
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_CommonCooldownInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_CommonCooldownInfo>(
		*CommonCooldownTag.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}
