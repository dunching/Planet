
#include "CharacterStateInfo.h"

#include "StateProcessorComponent.h"

FCharacterStateInfo::FCharacterStateInfo()
{

}

float FCharacterStateInfo::GetRemainTime() const
{
	return Duration - TotalTime;
}

float FCharacterStateInfo::GetRemainTimePercent() const
{
	return (Duration - TotalTime) / Duration;
}

bool FCharacterStateInfo_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FCharacterStateInfo_FASI_Container::AddItem(const TSharedPtr<FCharacterStateInfo>& CharacterStateInfoSPtr)
{
#if UE_EDITOR || UE_SERVER
	if (StateProcessorComponent->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			FItemType Item;

			Item.CharacterStateInfoSPtr = CharacterStateInfoSPtr;

			auto& Ref = Items.Add_GetRef(Item);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FCharacterStateInfo_FASI_Container::UpdateItem(const TSharedPtr<FCharacterStateInfo>& CharacterStateInfoSPtr)
{
#if UE_EDITOR || UE_SERVER
	if (StateProcessorComponent->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].CharacterStateInfoSPtr == CharacterStateInfoSPtr)
				{
					MarkItemDirty(Items[Index]);
					return;
				}
			}

			AddItem(CharacterStateInfoSPtr);
		}
	}
#endif
}

void FCharacterStateInfo_FASI_Container::RemoveItem(const TSharedPtr<FCharacterStateInfo>& CharacterStateInfoSPtr)
{
#if UE_EDITOR || UE_SERVER
	if (StateProcessorComponent->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); ++Index)
		{
			if (Items[Index].CharacterStateInfoSPtr == CharacterStateInfoSPtr)
			{
				Items.RemoveAt(Index);
				MarkArrayDirty();
				return;
			}
		}
	}
#endif
}

void FCharacterStateInfo_FASI::PreReplicatedRemove(const FCharacterStateInfo_FASI_Container& InArraySerializer)
{
	InArraySerializer.StateProcessorComponent->RemoveStateDisplay(CharacterStateInfoSPtr);
}

void FCharacterStateInfo_FASI::PostReplicatedAdd(const FCharacterStateInfo_FASI_Container& InArraySerializer)
{
	InArraySerializer.StateProcessorComponent->AddStateDisplay(CharacterStateInfoSPtr);
}

void FCharacterStateInfo_FASI::PostReplicatedChange(const FCharacterStateInfo_FASI_Container& InArraySerializer)
{
	InArraySerializer.StateProcessorComponent->ChangeStateDisplay(CharacterStateInfoSPtr);
}

bool FCharacterStateInfo_FASI::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		Ar << CharacterStateInfoSPtr->Duration;
		Ar << CharacterStateInfoSPtr->TotalTime;
		Ar << CharacterStateInfoSPtr->Num;
		Ar << CharacterStateInfoSPtr->Text;
		Ar << CharacterStateInfoSPtr->Tag;
		Ar << CharacterStateInfoSPtr->DefaultIcon;
	}
	else if (Ar.IsLoading())
	{
		if (!CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		}
		Ar << CharacterStateInfoSPtr->Duration;
		Ar << CharacterStateInfoSPtr->TotalTime;
		Ar << CharacterStateInfoSPtr->Num;
		Ar << CharacterStateInfoSPtr->Text;
		Ar << CharacterStateInfoSPtr->Tag;
		Ar << CharacterStateInfoSPtr->DefaultIcon;
	}

	return true;
}
