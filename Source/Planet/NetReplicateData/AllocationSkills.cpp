
#include "AllocationSkills.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitContainer.h"
#include "HoldingItemsComponent.h"

void FSocket_FASI::PreReplicatedRemove(const FAllocation_FASI_Container& InArraySerializer)
{

}

void FSocket_FASI::PostReplicatedAdd(const FAllocation_FASI_Container& InArraySerializer)
{
	// 在这里 我们对本地的数据进行绑定
	HoldingItemsComponentPtr = InArraySerializer.HoldingItemsComponentPtr;
	ProxySPtr = HoldingItemsComponentPtr->FindProxy(ProxyID);

	TSharedPtr<FSocket_FASI> SPtr = MakeShared<FSocket_FASI>();

	*SPtr = *this;

	InArraySerializer.UnitProxyProcessComponentPtr->Add(SPtr);
}

void FSocket_FASI::PostReplicatedChange(const FAllocation_FASI_Container& InArraySerializer)
{
	// 在这里 我们对本地的数据进行绑定
	HoldingItemsComponentPtr = InArraySerializer.HoldingItemsComponentPtr;
	ProxySPtr = HoldingItemsComponentPtr->FindProxy(ProxyID);

	TSharedPtr<FSocket_FASI> SPtr = MakeShared<FSocket_FASI>();

	*SPtr = *this;

	InArraySerializer.UnitProxyProcessComponentPtr->Update(SPtr);
}

bool FSocket_FASI::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto KeyStr = Key.ToString();
		Ar << KeyStr;
		Ar << Socket;
		Ar << ProxyID;
	}
	else if (Ar.IsLoading())
	{
		FString KeyStr;
		Ar << KeyStr;
		Key = FKey(*KeyStr);

		Ar << Socket;
		Ar << ProxyID;
	}

	return true;
}

bool FSocket_FASI::operator==(const FSocket_FASI& Right) const
{
	return true;
}

bool FAllocation_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FAllocation_FASI_Container::AddItem(const TSharedPtr<FItemType>& ProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (HoldingItemsComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			FItemType Item;

			Item = *ProxySPtr;

			auto& Ref = Items.Add_GetRef(Item);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FAllocation_FASI_Container::UpdateItem(const TSharedPtr<FItemType>& ProxySPtr)
{
#if UE_EDITOR || UE_SERVER
	if (HoldingItemsComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (ProxySPtr)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].Socket == ProxySPtr->Socket)
				{
					Items[Index] = *ProxySPtr;

					MarkItemDirty(Items[Index]);
					return;
				}
			}
		}
	}
#endif
}
