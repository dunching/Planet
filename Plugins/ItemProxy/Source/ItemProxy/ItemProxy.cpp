#include "ItemProxy.h"

#include "AbilitySystemComponent.h"
#include "InventoryComponentBase.h"
#include "ItemProxyVisitorBase.h"
#include "PAD_ItemProxyCollection.h"

#include "ItemProxy_Description.h"
#include "ItemProxy_Descriptions.h"

FBasicProxy::FBasicProxy()
{
}

FBasicProxy::~FBasicProxy()
{
}

bool FBasicProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Ar << ProxyType;
	Ar << ID;

	return true;
}

void FBasicProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	ProxyType = InProxyType;
	if (!ID.IsValid())
	{
		ID = FGuid::NewGuid();
	}
}

void FBasicProxy::UpdateByRemote(
	const TSharedPtr<FBasicProxy>& RemoteSPtr
	)
{
	GetInventoryComponentBase()->UpdateID(RemoteSPtr->ID, ID);
	ID = RemoteSPtr->ID;

	ProxyType = RemoteSPtr->ProxyType;
}

void FBasicProxy::ProcessProxyInteraction(
	EItemProxyInteractionType ItemProxyInteractionType
	)
{
}

TSet<EItemProxyInteractionType> FBasicProxy::GetInteractionsType() const
{
	return {};
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

void FBasicProxy::UpdateData()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponentBase()->GetNetMode() == NM_DedicatedServer)
	{
		GetInventoryComponentBase()->UpdateProxy(GetID());
	}
#endif
}

TObjectPtr<FBasicProxy::FItemProxy_Description> FBasicProxy::GetItemProxy_Description() const
{
	auto TableRowPtr = GetTableRowProxy();

	return TableRowPtr->ItemProxy_Description.LoadSynchronous();
}

void FBasicProxy::SetInventoryComponentBase(
	const TObjectPtr<UInventoryComponentBase>& NewInventoryComponentPtr
	)
{
	InventoryComponentPtr = NewInventoryComponentPtr;
}

TObjectPtr<UInventoryComponentBase>  FBasicProxy::GetInventoryComponentBase() const
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

const FTableRowProxy* FBasicProxy::GetTableRowProxy() const
{
	auto SceneProxyExtendInfoMapPtr = UPAD_ItemProxyCollection::GetInstance();
	return SceneProxyExtendInfoMapPtr->GetTableRowProxy(ProxyType);
}

void FBasicProxy::SetID(
	const IDType& NewID
	)
{
	ID = NewID;
}
