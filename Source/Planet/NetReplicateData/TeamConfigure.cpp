
#include "TeamConfigure.h"

#include "GroupMnaggerComponent.h"
#include "CharacterBase.h"
#include "HoldingItemsComponent.h"

bool FTeamConfigure::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	return true;
}

bool FTeamConfigure::Identical(const FTeamConfigure* Other, uint32 PortFlags) const
{
	return Other->bIsNotChanged;
}

void FTeammate_FASI::PreReplicatedRemove(const FTeammate_FASI_Container& InArraySerializer)
{

}

void FTeammate_FASI::PostReplicatedAdd(const FTeammate_FASI_Container& InArraySerializer)
{
	// 在这里 我们对本地的数据进行绑定
	OwnerCharacterPtr = InArraySerializer.OwnerCharacterPtr;
	CharacterProxySPtr =
		OwnerCharacterPtr->GetHoldingItemsComponent()->FindUnit_Character(CharacterProxyID);
}

void FTeammate_FASI::PostReplicatedChange(const FTeammate_FASI_Container& InArraySerializer)
{
	// 在这里 我们对本地的数据进行绑定
	OwnerCharacterPtr = InArraySerializer.OwnerCharacterPtr;
	CharacterProxySPtr = 
		OwnerCharacterPtr->GetHoldingItemsComponent()->FindUnit_Character(CharacterProxyID);
}

bool FTeammate_FASI::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		Ar << Index;
		Ar << CharacterProxyID;
	}
	else if (Ar.IsLoading())
	{
		Ar << Index;
		Ar << CharacterProxyID;
	}

	return true;
}

bool FTeammate_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FTeammate_FASI_Container::UpdateItem(const FItemType& Item)
{
#if UE_EDITOR || UE_SERVER
	if (OwnerCharacterPtr->GetLocalRole() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); Index++)
		{
			if (Items[Index].Index == Item.Index)
			{
				Items[Index] = Item;

				MarkItemDirty(Items[Index]);
				return;
			}
		}

		auto& Ref = Items.Add_GetRef(Item);

		MarkItemDirty(Ref);
	}
#endif
}
