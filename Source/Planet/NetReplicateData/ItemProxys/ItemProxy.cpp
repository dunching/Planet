
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
	Ar << ProxyType;
	Ar << ID;
	Ar << OwnerCharacter_ID;

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
	ID = RemoteSPtr->ID;
	OwnerCharacter_ID = RemoteSPtr->OwnerCharacter_ID;
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

FBasicProxy::IDType FBasicProxy::GetID()const
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

	return SceneProxyExtendInfoPtr->DefaultIcon;
}

void FBasicProxy::Update2Client()
{
	HoldingItemsComponentPtr->Proxy_Container.UpdateItem(GetID());
}

ACharacterBase* FBasicProxy::GetOwnerCharacter() const
{
	return GetOwnerCharacterProxy().Pin()->ProxyCharacterPtr.Get();
}

TWeakPtr<FCharacterProxy> FBasicProxy::GetOwnerCharacterProxy()const
{
	return HoldingItemsComponentPtr->FindProxy_Character(OwnerCharacter_ID);
}

FString FBasicProxy::GetProxyName() const
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy();

	return SceneProxyExtendInfoPtr->ProxyName;
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
}

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag)
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_CommonCooldownInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_CommonCooldownInfo>(*CommonCooldownTag.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

FToolProxy::FToolProxy()
{

}

int32 FToolProxy::GetNum() const
{
	return Num;
}

TWeakPtr<FCharacterProxy> FAllocationbleProxy::GetAllocationCharacterProxy()
{
	return HoldingItemsComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

TWeakPtr<FCharacterProxy> FAllocationbleProxy::GetAllocationCharacterProxy() const
{
	return HoldingItemsComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

void FAllocationbleProxy::SetAllocationCharacterProxy(
	const TSharedPtr < FCharacterProxy>& InAllocationCharacterProxyPtr, const FGameplayTag& InSocketTag
	)
{
	SocketTag = InSocketTag;
	if (InAllocationCharacterProxyPtr)
	{
		if (AllocationCharacter_ID == InAllocationCharacterProxyPtr->GetID())
		{
			return;
		}

		AllocationCharacter_ID = InAllocationCharacterProxyPtr->GetID();
	}
	else
	{
		AllocationCharacter_ID = FGuid();
	}

#if UE_EDITOR || UE_CLIENT
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (GetOwnerCharacter()->GetNetMode() == NM_Client)
	{
		HoldingItemsComponentPtr->SetAllocationCharacterProxy(this->GetID(), AllocationCharacter_ID, InSocketTag);
	}
#endif

	OnAllocationCharacterProxyChanged.ExcuteCallback(GetAllocationCharacterProxy());
}

FGameplayTag FAllocationbleProxy::GetCurrentSocketTag() const
{
	return  SocketTag;
}

FBasicProxy::IDType FAllocationbleProxy::GetAllocationCharacterID() const
{
	return  AllocationCharacter_ID;
}

ACharacterBase* FAllocationbleProxy::GetAllocationCharacter() const
{
	return GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr.Get();
}
