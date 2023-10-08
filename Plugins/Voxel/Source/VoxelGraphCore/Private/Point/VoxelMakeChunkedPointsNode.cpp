// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelMakeChunkedPointsNode.h"
#include "Point/VoxelPointSubsystem.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeChunkedPoints, Out)
{
	const TValue<int32> ChunkSize = Get(ChunkSizePin, Query);

	return VOXEL_ON_COMPLETE(ChunkSize)
	{
		// Don't use the Query runtime info, use the context one
		// (pooling being done per brush makes more sense)
		const TSharedRef<FVoxelQueryContext> Context = Query.GetSharedContext();

		const TSharedPtr<FVoxelPointSubsystem> Subsystem = Context->RuntimeInfo->FindSubsystem<FVoxelPointSubsystem>();
		if (!ensure(Subsystem))
		{
			return {};
		}

		VOXEL_SCOPE_LOCK(Subsystem->CriticalSection);

		FVoxelPointChunkProviderRef ChunkProviderRef;
		ChunkProviderRef.NodePath = Query.GetContext().Path;
		ChunkProviderRef.NodePath.NodeRefs.Add(GetNodeRef());
		ChunkProviderRef.RuntimeProvider = Context->RuntimeInfo->GetInstance();

		TSharedPtr<const FVoxelChunkedPointSet>& ChunkedPointSet = Subsystem->ChunkProviderToChunkedPointSet_RequiresLock.FindOrAdd(ChunkProviderRef);
		if (!ChunkedPointSet ||
			ChunkedPointSet->GetChunkSize() != ChunkSize)
		{
			ChunkedPointSet = MakeVoxelShared<FVoxelChunkedPointSet>(
				ChunkSize,
				ChunkProviderRef,
				Context,
				GetCompute(InPin, Context));
		}
		return ChunkedPointSet.ToSharedRef();
	};
}