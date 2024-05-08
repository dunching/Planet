// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "Toolkits/VoxelEditorToolkitImpl.h"
#include "Subsystems/AssetEditorSubsystem.h"

FVoxelToolkit::~FVoxelToolkit()
{
	if (GEditor)
	{
		GEditor->UnregisterForUndo(this);
	}
}

void FVoxelToolkit::InitializeInternal(const TSharedRef<FUICommandList>& Commands, UObject* Asset)
{
	GEditor->RegisterForUndo(this);

	const FObjectProperty* Property = GetObjectProperty();
	check(Property);
	check(Asset->IsA(Property->PropertyClass));
	CachedAssetPtr = Property->ContainerPtrToValuePtr<UObject*>(this);

	*CachedAssetPtr = Asset;

	PrivateCommands = Commands;
	PrivateTicker = MakeVoxelShared<FTicker>(*this);

	Initialize();
}

const FObjectProperty* FVoxelToolkit::GetObjectProperty() const
{
	return CastField<FObjectProperty>(GetStruct()->FindPropertyByName("Asset"));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelToolkit::SetTabManager(const TSharedRef<FTabManager>& TabManager)
{
	ensure(!WeakTabManager.IsValid());
	WeakTabManager = TabManager;
}

void FVoxelToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	AddStructReferencedObjects(Collector);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelToolkit* FVoxelToolkit::OpenToolkit(UObject* Asset, UScriptStruct* ToolkitStruct)
{
	const FObjectProperty* ObjectProperty = TVoxelInstancedStruct<FVoxelToolkit>(ToolkitStruct)->GetObjectProperty();
	if (!ensure(ObjectProperty))
	{
		return nullptr;
	}

	if (!ensure(Asset) ||
		!ensure(Asset->IsA(ObjectProperty->PropertyClass)))
	{
		return nullptr;
	}

	UObject* OuterAsset = Asset;
	while (OuterAsset && !Cast<UPackage>(OuterAsset->GetOuter()))
	{
		OuterAsset = OuterAsset->GetOuter();
	}

	if (!ensure(OuterAsset))
	{
		return nullptr;
	}

	const UScriptStruct* OuterToolkitStruct = FVoxelEditorToolkitImpl::GetToolkitStruct(OuterAsset->GetClass());
	if (!ensure(OuterToolkitStruct))
	{
		return nullptr;
	}

	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(OuterAsset);

	const TSharedPtr<IToolkit> ToolkitInterface = FToolkitManager::Get().FindEditorForAsset(OuterAsset);
	if (!ensure(ToolkitInterface) ||
		!ensure(ToolkitInterface->GetToolkitFName() == OuterToolkitStruct->GetFName()))
	{
		return nullptr;
	}

	ToolkitInterface->BringToolkitToFront();

	FVoxelEditorToolkitImpl& ToolkitImpl = static_cast<FVoxelEditorToolkitImpl&>(*ToolkitInterface);

	if (OuterAsset == Asset &&
		// GetToolkit will be null if we're using modes
		ToolkitImpl.GetToolkit())
	{
		if (!ensure(ToolkitImpl.ToolkitStruct == ToolkitStruct) ||
			!ensure(ToolkitImpl.GetToolkit()->GetStruct() == ToolkitStruct))
		{
			return nullptr;
		}

		return ToolkitImpl.GetToolkit().Get();
	}

	ToolkitImpl.SetCurrentMode(ToolkitStruct->GetFName());

	const TSharedPtr<FVoxelToolkitApplicationMode> Mode = StaticCastSharedPtr<FVoxelToolkitApplicationMode>(ToolkitImpl.GetCurrentModePtr());
	if (!ensure(Mode))
	{
		return nullptr;
	}

	const TSharedPtr<FVoxelToolkit> Toolkit = Mode->GetToolkit();
	if (!ensure(Toolkit) ||
		!ensure(Toolkit->GetStruct() == ToolkitStruct))
	{
		return nullptr;
	}

	return Toolkit.Get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelToolkit::NotifyPreChange(FProperty* PropertyAboutToChange)
{
	PreEditChange(PropertyAboutToChange);
}

void FVoxelToolkit::NotifyPreChange(FEditPropertyChain* PropertyAboutToChange)
{
	PreEditChange(PropertyAboutToChange->GetActiveNode()->GetValue());
}

void FVoxelToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	ensure(PropertyChangedEvent.Property == PropertyThatChanged);
	PostEditChange(PropertyChangedEvent);
}

void FVoxelToolkit::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FEditPropertyChain* PropertyThatChanged)
{
	PostEditChange(PropertyChangedEvent);
}