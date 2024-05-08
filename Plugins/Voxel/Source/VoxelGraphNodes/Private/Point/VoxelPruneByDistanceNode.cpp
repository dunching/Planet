// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPruneByDistanceNode.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_PruneByDistance, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);
	const TValue<float> Distance = Get(DistancePin, Query);

	return VOXEL_ON_COMPLETE(Points, Distance)
	{
		if (Points->Num() == 0)
		{
			return {};
		}
		if (Distance < KINDA_SMALL_NUMBER)
		{
			return Points;
		}

		VOXEL_SCOPE_COUNTER_FORMAT("PruneByDistance Num=%d", Points->Num());
		FVoxelNodeStatScope StatScope(*this, Points->Num());

		FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Position, FVoxelVectorBuffer, PositionBuffer);

		const float HalfDistance = Distance / 2;
		const float DistanceSquared = FMath::Square(Distance);
		const float BucketSize = Distance;
		const float InvBucketSize = 1. / BucketSize;

		struct FNode
		{
			int32 Next = -1;
			FVector3f Position = FVector3f(ForceInit);
		};
		TVoxelChunkedArray<FNode> Nodes;

		struct FBucket
		{
			int32 NodeIndex = -1;
		};

		TVoxelAddOnlyChunkedMap<FIntVector, FBucket> Map;
		Map.Reserve(8 * Points->Num());

		FVoxelInt32BufferStorage Indices;
		Indices.Reserve(Points->Num());

		for (int32 Index = 0; Index < Points->Num(); Index++)
		{
			const FVector3f Position = PositionBuffer[Index];

			TVoxelArray<FIntVector, TFixedAllocator<8>> Keys;
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(-HalfDistance, -HalfDistance, -HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(+HalfDistance, -HalfDistance, -HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(-HalfDistance, +HalfDistance, -HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(+HalfDistance, +HalfDistance, -HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(-HalfDistance, -HalfDistance, +HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(+HalfDistance, -HalfDistance, +HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(-HalfDistance, +HalfDistance, +HalfDistance)) * InvBucketSize));
			Keys.AddUnique(FVoxelUtilities::FloorToInt((Position + FVector3f(+HalfDistance, +HalfDistance, +HalfDistance)) * InvBucketSize));

			TVoxelArray<FBucket*, TFixedAllocator<8>> Buckets;
			for (const FIntVector& Key : Keys)
			{
				FBucket& Bucket = Map.FindOrAdd(Key);
				int32 NodeIndex = Bucket.NodeIndex;
				while (NodeIndex != -1)
				{
					const FNode& Node = Nodes[NodeIndex];
					if (FVector3f::DistSquared(Node.Position, Position) < DistanceSquared)
					{
						goto Skip;
					}
					NodeIndex = Node.Next;
				}
				Buckets.Add(&Bucket);
			}

			for (FBucket* Bucket : Buckets)
			{
				FNode NewNode;
				NewNode.Next = Bucket->NodeIndex;
				NewNode.Position = Position;
				Bucket->NodeIndex = Nodes.Add(NewNode);
			}

			Indices.Add(Index);

		Skip:
			;
		}

		return Points->Gather(FVoxelInt32Buffer::Make(Indices));
	};
}