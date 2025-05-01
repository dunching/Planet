#include "ItemProxy.h"

#include "ItemProxy_Minimal.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "Planet.h"
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

int32 FBasicProxy::GetNum() const
{
	return 1;
}

int32 FToolProxy::GetNum() const
{
	return FAllocationbleProxy::GetNum();
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
	InventoryComponentPtr->Proxy_Container.UpdateItem(GetID());
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

FAllocationbleProxy::FAllocationbleProxy()
{
}

bool FAllocationbleProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << AllocationCharacter_ID;
	Ar << SocketTag;

	return true;
}

void FAllocationbleProxy::UpdateByRemote(const TSharedPtr<FAllocationbleProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	AllocationCharacter_ID = RemoteSPtr->AllocationCharacter_ID;
	SocketTag = RemoteSPtr->SocketTag;

	OnAllocationCharacterProxyChanged(GetAllocationCharacterProxy());
}

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag)
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_CommonCooldownInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_CommonCooldownInfo>(
		*CommonCooldownTag.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

FToolProxy::FToolProxy()
{
}

TWeakPtr<FCharacterProxy> FAllocationbleProxy::GetAllocationCharacterProxy()
{
	return InventoryComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

TWeakPtr<FCharacterProxy> FAllocationbleProxy::GetAllocationCharacterProxy() const
{
	return InventoryComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

void FAllocationbleProxy::SetAllocationCharacterProxy(
	const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr, const FGameplayTag& InSocketTag
)
{
	// 这里做一个转发，
	// 同步到服务器
	if (InventoryComponentPtr->GetNetMode() == NM_Client)
	{
		if (InAllocationCharacterProxyPtr)
		{
			InventoryComponentPtr->SetAllocationCharacterProxy(GetID(), InAllocationCharacterProxyPtr->GetID(),
			                                                   InSocketTag);
		}
		else
		{
			InventoryComponentPtr->SetAllocationCharacterProxy(GetID(), FGuid(), InSocketTag);
		}
		// return;
	}

	if (InAllocationCharacterProxyPtr && InSocketTag.IsValid())
	{
		if (
			(AllocationCharacter_ID == InAllocationCharacterProxyPtr->GetID()) &&
			(SocketTag == InSocketTag)
		)
		{
			return;
		}

		// 找到这个物品之前被分配的插槽
		// 如果不是在同一个CharacterActor上，则需要取消分配
		if (AllocationCharacter_ID != InAllocationCharacterProxyPtr->GetID())
		{
			UnAllocation();
		}

		auto PreviousAllocationCharacterProxySPtr = InventoryComponentPtr->FindProxy_Character(AllocationCharacter_ID);
		if (PreviousAllocationCharacterProxySPtr)
		{
			auto CharacterSocket = PreviousAllocationCharacterProxySPtr->FindSocket(SocketTag);
			CharacterSocket.ResetAllocatedProxy();

			PreviousAllocationCharacterProxySPtr->UpdateSocket(CharacterSocket);
		}

		const auto PreviousAllocationCharacter_ID = AllocationCharacter_ID;
		AllocationCharacter_ID = InAllocationCharacterProxyPtr->GetID();
		SocketTag = InSocketTag;

		// 如果不是在同一个CharacterActor上，则需要重新分配
		// 否则重新分配
		if (PreviousAllocationCharacter_ID != InAllocationCharacterProxyPtr->GetID())
		{
			Allocation();
		}

		// 将这个物品注册到新的插槽
		auto CharacterSocket = InAllocationCharacterProxyPtr->FindSocket(SocketTag);
		CharacterSocket.SetAllocationedProxyID(GetID());

		InAllocationCharacterProxyPtr->UpdateSocket(CharacterSocket);

		OnAllocationCharacterProxyChanged.ExcuteCallback(GetAllocationCharacterProxy());

		Update2Client();
	}
	else
	{
		ResetAllocationCharacterProxy();
	}
}

void FAllocationbleProxy::ResetAllocationCharacterProxy()
{
	// 
	if (InventoryComponentPtr->GetNetMode() == NM_Client)
	{
		InventoryComponentPtr->SetAllocationCharacterProxy(GetID(), FGuid(), FGameplayTag::EmptyTag);
		// return;
	}

	// 找到这个物品之前被分配的插槽
	UnAllocation();

	auto PreviousAllocationCharacterProxySPtr = InventoryComponentPtr->FindProxy_Character(AllocationCharacter_ID);
	if (PreviousAllocationCharacterProxySPtr)
	{
		auto CharacterSocket = PreviousAllocationCharacterProxySPtr->FindSocket(SocketTag);
		CharacterSocket.ResetAllocatedProxy();

		PreviousAllocationCharacterProxySPtr->UpdateSocket(CharacterSocket);
	}

	AllocationCharacter_ID = FGuid();
	SocketTag = FGameplayTag::EmptyTag;

	OnAllocationCharacterProxyChanged.ExcuteCallback(GetAllocationCharacterProxy());

	Update2Client();
}

FGameplayTag FAllocationbleProxy::GetCurrentSocketTag() const
{
	return SocketTag;
}

FBasicProxy::IDType FAllocationbleProxy::GetAllocationCharacterID() const
{
	return AllocationCharacter_ID;
}

ACharacterBase* FAllocationbleProxy::GetAllocationCharacter() const
{
	auto AllocationCharacterProxySPtr = GetAllocationCharacterProxy();
	if (AllocationCharacterProxySPtr.IsValid())
	{
		return AllocationCharacterProxySPtr.Pin()->GetCharacterActor().Get();
	}
	return nullptr;
}
