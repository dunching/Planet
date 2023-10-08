// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "ActorFactoryVoxelActor.h"
#include "VoxelActor.h"
#include "VoxelGraphInterface.h"
#include "VoxelParameterContainer.h"

DEFINE_VOXEL_PLACEABLE_ITEM_FACTORY(UActorFactoryVoxelActor);

UActorFactoryVoxelActor::UActorFactoryVoxelActor()
{
	DisplayName = INVTEXT("Voxel Actor");
	NewActorClass = AVoxelActor::StaticClass();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UActorFactoryVoxelActor_FromGraph::UActorFactoryVoxelActor_FromGraph()
{
	DisplayName = INVTEXT("Voxel Actor");
	NewActorClass = AVoxelActor::StaticClass();
}

bool UActorFactoryVoxelActor_FromGraph::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	return
		AssetData.IsValid() &&
		AssetData.GetClass()->IsChildOf<UVoxelGraphInterface>();
}

void UActorFactoryVoxelActor_FromGraph::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	AVoxelActor* SceneActor = CastChecked<AVoxelActor>(NewActor);
	SceneActor->SetGraph(CastChecked<UVoxelGraphInterface>(Asset));
	SceneActor->QueueRecreate();
}

UObject* UActorFactoryVoxelActor_FromGraph::GetAssetFromActorInstance(AActor* ActorInstance)
{
	return CastChecked<AVoxelActor>(ActorInstance)->GetGraph();
}