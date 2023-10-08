// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphInterfaceToolkit.h"
#include "VoxelGraphToolkit.h"
#include "VoxelGraphInstance.h"
#include "VoxelActor.h"
#include "VoxelRuntime.h"

TArray<FVoxelToolkit::FMode> FVoxelGraphInterfaceToolkit::GetModes() const
{
	if (UVoxelGraphInstance* Instance = Cast<UVoxelGraphInstance>(Asset))
	{
		FMode Mode;
		Mode.Struct = FVoxelGraphInterfacePreviewToolkit::StaticStruct();
		Mode.Object = Instance;
		return { Mode };
	}

	if (const UVoxelGraph* Template = Cast<UVoxelGraph>(Asset))
	{
		FMode Mode;
		Mode.Struct = FVoxelGraphToolkit::StaticStruct();
		Mode.Object = Template->GetGraph();
		return { Mode };
	}

	return {};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphInterfacePreviewToolkit::~FVoxelGraphInterfacePreviewToolkit()
{
	VOXEL_FUNCTION_COUNTER();

	for (AActor* Actor : PrivateActors)
	{
		if (!ensure(Actor))
		{
			continue;
		}

		Actor->Destroy();
	}
}

void FVoxelGraphInterfacePreviewToolkit::SetupPreview()
{
	VOXEL_FUNCTION_COUNTER();

	Super::SetupPreview();

	CachedWorld = GetPreviewScene().GetWorld();
	if (!ensure(CachedWorld.IsValid()))
	{
		return;
	}

	AActor* Actor = SpawnActor<AActor>();
	if (!ensure(Actor))
	{
		return;
	}

	PrivateRootComponent = NewObject<USceneComponent>(Actor, NAME_None, RF_Transient);
	if (!ensure(PrivateRootComponent))
	{
		return;
	}
	PrivateRootComponent->RegisterComponent();

	GetPreviewScene().AddComponent(PrivateRootComponent, FTransform::Identity);
	Actor->SetRootComponent(PrivateRootComponent);

	VoxelActor = SpawnActor<AVoxelActor>();
	if (!ensure(VoxelActor))
	{
		return;
	}

	VoxelActor->SetGraph(Asset);
	VoxelActor->DestroyRuntime();
	VoxelActor->CreateRuntime();
}

TOptional<float> FVoxelGraphInterfacePreviewToolkit::GetInitialViewDistance() const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(VoxelActor))
	{
		return {};
	}

	const TSharedPtr<FVoxelRuntime> Runtime = VoxelActor->GetRuntime();
	if (!ensure(Runtime))
	{
		return {};
	}

	const FVoxelOptionalBox Bounds = Runtime->GetBounds();
	if (!Bounds.IsValid())
	{
		return {};
	}

	return Bounds.GetBox().Size().Length();
}