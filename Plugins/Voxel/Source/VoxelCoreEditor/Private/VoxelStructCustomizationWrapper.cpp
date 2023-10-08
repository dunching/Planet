// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelStructCustomizationWrapper);

class FVoxelStructCustomizationWrapperTicker : public FVoxelEditorSingleton
{
public:
	TArray<TWeakPtr<FVoxelStructCustomizationWrapper>> WeakWrappers;

	//~ Begin FVoxelEditorSingleton Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();
		check(IsInGameThread());

		WeakWrappers.RemoveAllSwap([](const TWeakPtr<FVoxelStructCustomizationWrapper>& WeakChildWrapper)
		{
			return !WeakChildWrapper.IsValid();
		});

		const double Time = FPlatformTime::Seconds();
		for (const TWeakPtr<FVoxelStructCustomizationWrapper>& WeakWrapper : WeakWrappers)
		{
			const TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper = WeakWrapper.Pin();
			if (!ensure(Wrapper.IsValid()))
			{
				continue;
			}

			if (Time < Wrapper->LastSyncTime + 0.1)
			{
				continue;
			}
			Wrapper->LastSyncTime = Time;

			// Tricky: can tick once after the property is gone due to SListPanel being delayed
			Wrapper->SyncFromSource();
		}
	}
	//~ End FVoxelEditorSingleton Interface
};
FVoxelStructCustomizationWrapperTicker* GVoxelStructCustomizationWrapperTicker = MakeVoxelSingleton(FVoxelStructCustomizationWrapperTicker);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelStructCustomizationWrapper> FVoxelStructCustomizationWrapper::Make(const TSharedRef<IPropertyHandle>& StructHandle)
{
	FVoxelEditorUtilities::TrackHandle(StructHandle);

	if (!ensure(StructHandle->IsValidHandle()))
	{
		return nullptr;
	}

	bool bHasValidStruct = true;
	TOptional<const UScriptStruct*> Struct;
	FVoxelEditorUtilities::ForeachData<FVoxelInstancedStruct>(StructHandle, [&](const FVoxelInstancedStruct& InstancedStruct)
	{
		if (!Struct.IsSet())
		{
			Struct = InstancedStruct.GetScriptStruct();
			return;
		}

		if (Struct.GetValue() != InstancedStruct.GetScriptStruct())
		{
			bHasValidStruct = false;
		}
	});

	if (!ensure(Struct.IsSet()) ||
		!bHasValidStruct ||
		!Struct.GetValue())
	{
		return nullptr;
	}

	const TSharedRef<FStructOnScope> StructOnScope = MakeVoxelShared<FStructOnScope>(Struct.GetValue());

	// Make sure the struct also has a valid package set, so that properties that rely on this (like FText) work correctly
	{
		TArray<UPackage*> OuterPackages;
		StructHandle->GetOuterPackages(OuterPackages);
		if (OuterPackages.Num() > 0)
		{
			StructOnScope->SetPackage(OuterPackages[0]);
		}
	}

	const TSharedRef<FVoxelStructCustomizationWrapper> Result(new FVoxelStructCustomizationWrapper(StructHandle, StructOnScope));
	GVoxelStructCustomizationWrapperTicker->WeakWrappers.Add(Result);
	Result->SyncFromSource();
	return Result;
}

TArray<TSharedPtr<IPropertyHandle>> FVoxelStructCustomizationWrapper::AddChildStructure()
{
	const TArray<TSharedPtr<IPropertyHandle>> ChildHandles = StructHandle->AddChildStructure(StructOnScope);
	for (const TSharedPtr<IPropertyHandle>& ChildHandle : ChildHandles)
	{
		SetupChildHandle(ChildHandle);
	}
	return ChildHandles;
}

IDetailPropertyRow* FVoxelStructCustomizationWrapper::AddExternalStructure(
	const FVoxelDetailInterface& DetailInterface,
	const FAddPropertyParams& Params)
{
	IDetailPropertyRow* Row = DetailInterface.AddExternalStructure(StructOnScope, Params);
	if (!ensure(Row))
	{
		return nullptr;
	}

	for (const TSharedPtr<IPropertyHandle>& ChildHandle : FVoxelEditorUtilities::GetChildHandlesRecursive(Row->GetPropertyHandle()))
	{
		SetupChildHandle(ChildHandle);
	}

	return Row;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelStructCustomizationWrapper::SyncFromSource() const
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelEditorUtilities::ForeachDataPtr<FVoxelInstancedStruct>(StructHandle, [&](FVoxelInstancedStruct* InstancedStruct)
	{
		if (!InstancedStruct)
		{
			// Will happen when removing array entries
			return;
		}

		if (!ensureVoxelSlow(InstancedStruct->GetScriptStruct() == StructOnScope->GetStruct()))
		{
			return;
		}

		if (InstancedStruct->IsA<FVoxelVirtualStruct>())
		{
			reinterpret_cast<FVoxelVirtualStruct*>(StructOnScope->GetStructMemory())->SyncFromSource_EditorOnly(InstancedStruct->Get<FVoxelVirtualStruct>());
		}
		else
		{
			InstancedStruct->GetScriptStruct()->CopyScriptStruct(StructOnScope->GetStructMemory(), InstancedStruct->GetStructMemory());
		}
	});
}

void FVoxelStructCustomizationWrapper::SyncToSource() const
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelEditorUtilities::ForeachData<FVoxelInstancedStruct>(StructHandle, [&](FVoxelInstancedStruct& InstancedStruct)
	{
		if (!ensureVoxelSlow(InstancedStruct.GetScriptStruct() == StructOnScope->GetStruct()))
		{
			return;
		}

		if (InstancedStruct.IsA<FVoxelVirtualStruct>())
		{
			reinterpret_cast<const FVoxelVirtualStruct*>(StructOnScope->GetStructMemory())->SyncToSource_EditorOnly(InstancedStruct.Get<FVoxelVirtualStruct>());
		}
		else
		{
			InstancedStruct.GetScriptStruct()->CopyScriptStruct(InstancedStruct.GetStructMemory(), StructOnScope->GetStructMemory());
		}
	});
}

void FVoxelStructCustomizationWrapper::SetupChildHandle(const TSharedPtr<IPropertyHandle>& Handle)
{
	if (!ensure(Handle))
	{
		return;
	}

	// Forward instance metadata, used to avoid infinite recursion in inline graphs
	for (TSharedPtr<IPropertyHandle> ParentHandle = StructHandle; ParentHandle; ParentHandle = ParentHandle->GetParentHandle())
	{
		if (const TMap<FName, FString>* Map = ParentHandle->GetInstanceMetaDataMap())
		{
			for (auto& It : *Map)
			{
				if (It.Key.ToString().StartsWith("Recursive_"))
				{
					Handle->SetInstanceMetaData(It.Key, It.Value);
				}
			}
		}
	}

	const FSimpleDelegate PreChangeDelegate = MakeWeakPtrDelegate(this, [this]
	{
		VOXEL_SCOPE_COUNTER("NotifyPreChange");
		StructHandle->NotifyPreChange();
	});
	const TDelegate<void(const FPropertyChangedEvent&)> PostChangeDelegate = MakeWeakPtrDelegate(this, [this](const FPropertyChangedEvent& PropertyChangedEvent)
	{
		// Critical to not have an exponential number of PostChange fired
		// StructHandle->NotifyPostChange will call the PostChangeDelegates of child struct customization
		if (LastPostChangeFrame == GFrameCounter &&
			PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
		{
			return;
		}
		LastPostChangeFrame = GFrameCounter;

		SyncToSource();

		{
			VOXEL_SCOPE_COUNTER("NotifyPostChange");
			StructHandle->NotifyPostChange(PropertyChangedEvent.ChangeType);
		}

		if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		{
			VOXEL_SCOPE_COUNTER("NotifyFinishedChangingProperties");
			StructHandle->NotifyFinishedChangingProperties();
		}
	});

	Handle->SetOnPropertyValuePreChange(PreChangeDelegate);
	Handle->SetOnPropertyValueChangedWithData(PostChangeDelegate);

	Handle->SetOnChildPropertyValuePreChange(PreChangeDelegate);
	Handle->SetOnChildPropertyValueChangedWithData(PostChangeDelegate);
}