#include "InventoryComponentBase.h"

#include <GameFramework/PlayerState.h>

#include "ItemProxy.h"
#include "Net/UnrealNetwork.h"

#include "ItemProxy_Container.h"
#include "ItemProxy_GenericType.h"
#include "ModifyItemProxyStrategyInterface.h"

UInventoryComponentBase::UInventoryComponentBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UInventoryComponentBase::UpdateID(
	const FGuid& NewID,
	const FGuid& OldID
	)
{
	if (auto Iter = ProxysMap.Find(OldID))
	{
		ProxysMap.Remove(OldID);
		ProxysMap.Add(NewID, *Iter);
	}
}

void UInventoryComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		Proxy_Container.SetInventoryComponentBase( this);
	}

	// 初始化一些需要的Proxy

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

void UInventoryComponentBase::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

void UInventoryComponentBase::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Proxy_Container, COND_None);
}

void UInventoryComponentBase::AddProxy_Pending(
	FGameplayTag ProxyType,
	int32 Num,
	FGuid Guid
	)
{
	if (PendingMap.Contains(Guid))
	{
		if (PendingMap[Guid].Contains(ProxyType))
		{
			PendingMap[Guid][ProxyType] += Num;
		}
		else
		{
			PendingMap[Guid].Add(ProxyType, Num);
		}
	}
	else
	{
		PendingMap.Add(Guid, {{ProxyType, Num}});
	}
}

void UInventoryComponentBase::SyncPendingProxy(
	FGuid Guid
	)
{
	if (PendingMap.Contains(Guid))
	{
		for (const auto& Iter : PendingMap)
		{
			for (const auto& SecondIter : Iter.Value)
			{
				AddProxyNum(SecondIter.Key, SecondIter.Value);
			}
		}
		PendingMap.Remove(Guid);
	}
}

FName UInventoryComponentBase::ComponentName = TEXT("InventoryComponent");

#if UE_EDITOR || UE_CLIENT
TSharedPtr<FBasicProxy> UInventoryComponentBase::AddProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	TSharedPtr<FBasicProxy> Result;

	const auto ProxyType = ProxySPtr->GetProxyType();
	ProxySPtr->InventoryComponentPtr = this;

	if (Result = FindProxy(ProxySPtr->GetID()))
	{
		return Result;
	}

	return Result;
}

void UInventoryComponentBase::RemoveProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	const auto ProxyType = ProxySPtr->GetProxyType();
}

void UInventoryComponentBase::UpdateProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	if (!(LocalProxySPtr && RemoteProxySPtr))
	{
		return;
	}

	if (LocalProxySPtr->GetProxyType() != RemoteProxySPtr->GetProxyType())
	{
		return;
	}

	const auto ProxyType = RemoteProxySPtr->GetProxyType();
}

#endif

void UInventoryComponentBase::UpdateInContainer(
	const TSharedPtr<FBasicProxy>& ItemProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.AddItem(ItemProxySPtr);
	}
#endif
}

void UInventoryComponentBase::AddToContainer(
	const TSharedPtr<FBasicProxy>& ItemProxySPtr
	)
{
	ItemProxySPtr->InventoryComponentPtr = this;

	ProxysAry.Add(ItemProxySPtr);
	ProxysMap.Add(ItemProxySPtr->ID, ItemProxySPtr);

	Proxy_Container.AddItem(ItemProxySPtr);

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.AddItem(ItemProxySPtr);
	}
#endif
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponentBase::GetProxys() const
{
	return ProxysAry;
}

void UInventoryComponentBase::AddProxys_Server_Implementation(
	const FGuid& RewardsItemID
	)
{
}

const TArray<TSharedPtr<FBasicProxy>>& UInventoryComponentBase::GetSceneUintAry() const
{
	return ProxysAry;
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponentBase::AddProxyNum(
	const FGameplayTag& ProxyType,
	int32 Num
	)
{
	if (Num <= 0)
	{
		return {};
	}

	TArray<TSharedPtr<FBasicProxy>> ResultAry;

	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	return ResultAry;
}

TSharedPtr<FBasicProxy> UInventoryComponentBase::AddProxy(
	const FGameplayTag& ProxyType
	)
{
	auto Ary = AddProxyNum(ProxyType, 1);
	if (Ary.IsValidIndex(0))
	{
		return Ary[0];
	}

	return nullptr;
}

void UInventoryComponentBase::RemoveProxyNum(
	const IDType& ID,
	int32 Num
	)
{
	if (Num <= 0)
	{
		return;
	}

	TSharedPtr<FBasicProxy> ResultSPtr = FindProxy(ID);
	if (ResultSPtr)
	{
		const auto ProxyType = ResultSPtr->GetProxyType();
	}
}

TSharedPtr<FBasicProxy> UInventoryComponentBase::FindProxy(
	const IDType& ID
	) const
{
	if (ProxysMap.Contains(ID))
	{
		return ProxysMap[ID];
	}
	
	return nullptr;
}

TSharedPtr<FBasicProxy> UInventoryComponentBase::FindProxyType(
	const FGameplayTag& ProxyType
	) const
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		return ProxyTypeMap[ProxyType];
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponentBase::GetProxys(
	const FGameplayTag& ProxyType
	) const
{
	TArray<TSharedPtr<FBasicProxy>> Result;

	for (const auto& Iter : ProxysAry)
	{
		if (Iter && Iter->GetProxyType().MatchesTag(ProxyType))
		{
			Result.Add(Iter);
		}
	}

	return Result;
}
