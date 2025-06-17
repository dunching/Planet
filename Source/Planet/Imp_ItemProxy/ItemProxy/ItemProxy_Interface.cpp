#include "ItemProxy_Interface.h"

#include "InventoryComponent.h"
#include "InventoryComponentBase.h"
#include "ItemProxy_Character.h"
#include "ModifyItemProxyStrategy.h"
#include "ModifyItemProxyStrategyInterface.h"

IProxy_SkillState::~IProxy_SkillState()
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
	OffsetNum = RemoteSPtr->OffsetNum;

	CallbackContainerHelper.ValueChanged(Num, RemoteSPtr->OffsetNum);
}

void IProxy_Unique::ModifyNum(
	int32 Value
	)
{
	const auto Old = Value;
	Num += Value;

	OffsetNum = Value;

	CallbackContainerHelper.ValueChanged(Old, OffsetNum);
}

int32 IProxy_Unique::GetOffsetNum() const
{
	return OffsetNum;
}

int32 IProxy_Unique::GetMaxNum() const
{
	return MaxNum;
}

int32 IProxy_Unique::GetNum() const
{
	return Num;
}

int32 GetProxyNum(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	auto Num = 1;
	if (ProxySPtr)
	{
		auto IProxy_UniquePtr = DynamicCastSharedPtr<IProxy_Unique>(ProxySPtr);
		if (IProxy_UniquePtr)
		{
			Num = IProxy_UniquePtr->GetNum();
		}
	}
	return Num;
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
	if (ProxyPtr->GetInventoryComponentBase()->GetNetMode() == NM_Client)
	{
		ProxyPtr->GetInventoryComponent()->
				  SetAllocationCharacterProxy(ProxyPtr->GetID(), FGuid(), FGameplayTag::EmptyTag);
	}

	// 找到这个物品之前被分配的插槽
	ProxyPtr->UnAllocation();

	auto PreviousAllocationCharacterProxySPtr = ProxyPtr->GetInventoryComponentBase()->FindProxy<FModifyItemProxyStrategy_Character>(
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

	ProxyPtr->UpdateData();
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
		return AllocationCharacterProxySPtr->GetCharacterActor().Get();
	}
	return nullptr;
}

TSharedPtr<FCharacterProxy> IProxy_Allocationble::GetAllocationCharacterProxy()
{
	return ProxyPtr->GetInventoryComponentBase()->FindProxy<FModifyItemProxyStrategy_Character>(AllocationCharacter_ID);
}

TSharedPtr<FCharacterProxy> IProxy_Allocationble::GetAllocationCharacterProxy() const
{
	return ProxyPtr->GetInventoryComponentBase()->FindProxy<FModifyItemProxyStrategy_Character>(AllocationCharacter_ID);
}

void IProxy_Allocationble::SetAllocationCharacterProxy(
	const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
	const FGameplayTag& InSocketTag
	)
{
	// 这里做一个转发，
	// 同步到服务器
	if (ProxyPtr->GetInventoryComponentBase()->GetNetMode() == NM_Client)
	{
		if (InAllocationCharacterProxyPtr)
		{
			ProxyPtr->GetInventoryComponent()->SetAllocationCharacterProxy(
																		 ProxyPtr->GetID(),
																		 InAllocationCharacterProxyPtr->GetID(),
																		 InSocketTag
																		);
		}
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

		ProxyPtr->UnAllocation();

		const auto PreviousAllocationCharacter_ID = AllocationCharacter_ID;
		AllocationCharacter_ID = InAllocationCharacterProxyPtr->GetID();
		SocketTag = InSocketTag;

		ProxyPtr->Allocation();

		OnAllocationCharacterProxyChanged.ExcuteCallback(GetAllocationCharacterProxy());

		ProxyPtr->UpdateData();
	}
	else
	{
		ResetAllocationCharacterProxy();
	}
}
