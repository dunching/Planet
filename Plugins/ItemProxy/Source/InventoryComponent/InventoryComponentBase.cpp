#include "InventoryComponentBase.h"

#include <GameFramework/PlayerState.h>

#include "ItemProxy.h"
#include "Net/UnrealNetwork.h"

#include "ItemProxy_Container.h"
#include "ItemProxy_GenericType.h"
#include "ModifyItemProxyStrategyInterface.h"
#include "Tools.h"

UInventoryComponentBase::UInventoryComponentBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UInventoryComponentBase::ProcessProxyInteraction(
	const FGuid& ProxyID,
	EItemProxyInteractionType ItemProxyInteractionType
	)
{
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		ProcessProxyInteraction_Server(ProxyID, ItemProxyInteractionType);
	}
	auto TargetProxySPtr = FindProxy(ProxyID);
	if (TargetProxySPtr)
	{
		TargetProxySPtr->ProcessProxyInteraction(ItemProxyInteractionType);
	}
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

void UInventoryComponentBase::ProcessProxyInteraction_Server_Implementation(
	const FGuid& ProxyID,
	EItemProxyInteractionType ItemProxyInteractionType
	)
{
	ProcessProxyInteraction(ProxyID, ItemProxyInteractionType);
}

void UInventoryComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		Proxy_Container.SetInventoryComponentBase(this);
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
	const TSharedPtr<FBasicProxy>& CacheProxySPtr
	)
{
	CacheProxySPtr->InventoryComponentPtr = this;

	if (auto Result = FindProxy(CacheProxySPtr->GetID()))
	{
		return Result;
	}

	const auto ProxyType = CacheProxySPtr->GetProxyType();
	for (const auto& Iter : ModifyItemProxyStrategiesMap)
	{
		if (ProxyType.MatchesTag(Iter.Key))
		{
			return Iter.Value->AddByRemote(this, CacheProxySPtr);
		}
	}

	return nullptr;
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

	for (const auto& Iter : ModifyItemProxyStrategiesMap)
	{
		if (ProxyType.MatchesTag(Iter.Key))
		{
			Iter.Value->UpdateByRemote(this, LocalProxySPtr, RemoteProxySPtr);
			return;
		}
	}
}

void UInventoryComponentBase::RemoveProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	if (auto Result = FindProxy(ProxySPtr->GetID()))
	{
		const auto ProxyType = ProxySPtr->GetProxyType();
		for (const auto& Iter : ModifyItemProxyStrategiesMap)
		{
			if (ProxyType.MatchesTag(Iter.Key))
			{
				Iter.Value->RemoveByRemote(this, ProxySPtr);
				return;
			}
		}
	}
}

#endif

void UInventoryComponentBase::AddToContainer(
	const TSharedPtr<FBasicProxy>& ItemProxySPtr
	)
{
	ItemProxySPtr->InventoryComponentPtr = this;

	ProxysAry.Add(ItemProxySPtr);
	ProxysMap.Add(ItemProxySPtr->ID, ItemProxySPtr);

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.AddItem(ItemProxySPtr);
	}
#endif
}

void UInventoryComponentBase::UpdateInContainer(
	const TSharedPtr<FBasicProxy>& ItemProxySPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.UpdateItem(ItemProxySPtr);
	}
#endif
}

void UInventoryComponentBase::RemoveFromContainer(
	const TSharedPtr<FBasicProxy>& ItemProxySPtr
	)
{
	for (int32 Index = 0; Index < ProxysAry.Num(); Index++)
	{
		if (ProxysAry[Index] == ItemProxySPtr)
		{
			ProxysAry.RemoveAt(Index);
			break;
		}
	}

	ProxysMap.Remove(ItemProxySPtr->ID);

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.RemoveItem(ItemProxySPtr);
	}
#endif
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponentBase::GetProxys() const
{
	return ProxysAry;
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
	for (const auto& Iter : ModifyItemProxyStrategiesMap)
	{
		if (ProxyType.MatchesTag(Iter.Key))
		{
			return Iter.Value->Add(this, ProxyType, Num);
		}
	}

	return {};
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

void UInventoryComponentBase::UpdateProxy(
	const IDType& ID
	)
{
	TSharedPtr<FBasicProxy> ResultSPtr = FindProxy(ID);
	if (ResultSPtr)
	{
		const auto ProxyType = ResultSPtr->GetProxyType();

		for (const auto& Iter : ModifyItemProxyStrategiesMap)
		{
			if (ProxyType.MatchesTag(Iter.Key))
			{
				Iter.Value->Update(this, ID);
				return;
			}
		}
	}
}

void UInventoryComponentBase::RemoveProxy(
	const IDType& ID
	)
{
	TSharedPtr<FBasicProxy> ResultSPtr = FindProxy(ID);
	if (ResultSPtr)
	{
		const auto ProxyType = ResultSPtr->GetProxyType();

		for (const auto& Iter : ModifyItemProxyStrategiesMap)
		{
			if (ProxyType.MatchesTag(Iter.Key))
			{
				Iter.Value->RemoveItemProxy(this, ID);
				return;
			}
		}
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
	for (const auto& Iter : ModifyItemProxyStrategiesMap)
	{
		if (ProxyType.MatchesTag(Iter.Key))
		{
			const auto Ary = Iter.Value->FindByType(ProxyType, this);
			if (Ary.IsValidIndex(0))
			{
				return Ary[0];
			}
		}
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponentBase::FindAllProxyType(
	const FGameplayTag& ProxyType
	) const
{
	TArray<TSharedPtr<FBasicProxy>> Result;
	for (const auto& Iter : ModifyItemProxyStrategiesMap)
	{
		if (ProxyType.MatchesTag(Iter.Key))
		{
			Result = Iter.Value->FindByType(ProxyType, this);
		}
	}

	return Result;
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

const TMap<FGameplayTag, TSharedPtr<FModifyItemProxyStrategyIterface>>& UInventoryComponentBase::
GetModifyItemProxyStrategies() const
{
	return ModifyItemProxyStrategiesMap;
}
