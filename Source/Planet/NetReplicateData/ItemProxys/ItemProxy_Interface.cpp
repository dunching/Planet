#include "ItemProxy_Interface.h"

#include "InventoryComponent.h"
#include "ItemProxy_Character.h"

IProxy_Cooldown::~IProxy_Cooldown()
{
}

IProxy_Unique::~IProxy_Unique()
{
}

bool IProxy_Unique::NetSerialize_Unique(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Ar << Num;
	Ar << OffsetNum;

	return true;
}

void IProxy_Unique::UpdateByRemote_Unique(
	const TSharedPtr<IProxy_Unique>& RemoteSPtr
	)
{
	Num = RemoteSPtr->Num;

	CallbackContainerHelper.ValueChanged(Num, RemoteSPtr->Num);
	OffsetNum = RemoteSPtr->OffsetNum;
}

void IProxy_Unique::AddNum(
	int32 Value
	)
{
	const auto Old = Value;
	Num += Value;

	OffsetNum = Value;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 IProxy_Unique::GetOffsetNum() const
{
	return OffsetNum;
}

IProxy_Allocationble::~IProxy_Allocationble()
{
}

bool IProxy_Allocationble::NetSerialize_Allocationble(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Ar << AllocationCharacter_ID;
	Ar << SocketTag;

	return true;
}

void IProxy_Allocationble::UpdateByRemote_Allocationble(
	const TSharedPtr<IProxy_Allocationble>& RemoteSPtr
	)
{
	AllocationCharacter_ID = RemoteSPtr->AllocationCharacter_ID;
	SocketTag = RemoteSPtr->SocketTag;

	OnAllocationCharacterProxyChanged(GetAllocationCharacterProxy());
}

void IProxy_Allocationble::ResetAllocationCharacterProxy()
{
	// 
	if (ProxyPtr->InventoryComponentPtr->GetNetMode() == NM_Client)
	{
		ProxyPtr->InventoryComponentPtr->
		          SetAllocationCharacterProxy(ProxyPtr->GetID(), FGuid(), FGameplayTag::EmptyTag);
		// return;
	}

	// 找到这个物品之前被分配的插槽
	ProxyPtr->UnAllocation();

	auto PreviousAllocationCharacterProxySPtr = ProxyPtr->InventoryComponentPtr->FindProxy_Character(
		 AllocationCharacter_ID
		);
	if (PreviousAllocationCharacterProxySPtr)
	{
		auto CharacterSocket = PreviousAllocationCharacterProxySPtr->FindSocket(SocketTag);
		CharacterSocket.ResetAllocatedProxy();

		PreviousAllocationCharacterProxySPtr->UpdateSocket(CharacterSocket);
	}

	AllocationCharacter_ID = FGuid();
	SocketTag = FGameplayTag::EmptyTag;

	OnAllocationCharacterProxyChanged.ExcuteCallback(GetAllocationCharacterProxy());

	ProxyPtr->Update2Client();
}

FGameplayTag IProxy_Allocationble::GetCurrentSocketTag() const
{
	return SocketTag;
}

FGuid IProxy_Allocationble::GetAllocationCharacterID() const
{
	return AllocationCharacter_ID;
}

void IProxy_Allocationble::SetCurrentSocketTag(
	const FGameplayTag& Socket
	)
{
	SocketTag = Socket;
}

void IProxy_Allocationble::SetAllocationCharacterID(
	const FGuid& ID
	)
{
	AllocationCharacter_ID = ID;
}

ACharacterBase* IProxy_Allocationble::GetAllocationCharacter() const
{
	auto AllocationCharacterProxySPtr = GetAllocationCharacterProxy();
	if (AllocationCharacterProxySPtr.IsValid())
	{
		return AllocationCharacterProxySPtr.Pin()->GetCharacterActor().Get();
	}
	return nullptr;
}

TWeakPtr<FCharacterProxy> IProxy_Allocationble::GetAllocationCharacterProxy()
{
	return ProxyPtr->InventoryComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

TWeakPtr<FCharacterProxy> IProxy_Allocationble::GetAllocationCharacterProxy() const
{
	return ProxyPtr->InventoryComponentPtr->FindProxy_Character(AllocationCharacter_ID);
}

void IProxy_Allocationble::SetAllocationCharacterProxy(
	const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
	const FGameplayTag& InSocketTag
	)
{
	// 这里做一个转发，
	// 同步到服务器
	if (ProxyPtr->InventoryComponentPtr->GetNetMode() == NM_Client)
	{
		if (InAllocationCharacterProxyPtr)
		{
			ProxyPtr->InventoryComponentPtr->SetAllocationCharacterProxy(
			                                                             ProxyPtr->GetID(),
			                                                             InAllocationCharacterProxyPtr->GetID(),
			                                                             InSocketTag
			                                                            );
		}
		else
		{
			ProxyPtr->InventoryComponentPtr->SetAllocationCharacterProxy(ProxyPtr->GetID(), FGuid(), InSocketTag);
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
			ProxyPtr->UnAllocation();
		}

		auto PreviousAllocationCharacterProxySPtr = ProxyPtr->InventoryComponentPtr->FindProxy_Character(
			 AllocationCharacter_ID
			);
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
			ProxyPtr->Allocation();
		}

		// 将这个物品注册到新的插槽
		auto CharacterSocket = InAllocationCharacterProxyPtr->FindSocket(SocketTag);
		CharacterSocket.SetAllocationedProxyID(ProxyPtr->GetID());

		InAllocationCharacterProxyPtr->UpdateSocket(CharacterSocket);

		OnAllocationCharacterProxyChanged.ExcuteCallback(GetAllocationCharacterProxy());

		ProxyPtr->Update2Client();
	}
	else
	{
		ResetAllocationCharacterProxy();
	}
}
