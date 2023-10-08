// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelMakePointsNode.h"

void FVoxelMakePointInitializer::ComputeRuntimeData()
{
	for (FVoxelMakePointPoint& Point : Points)
	{
		Point.RuntimeMesh = FVoxelStaticMesh::Make(Point.Mesh);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakePoints, Out)
{
	const TValue<FVoxelMakePointInitializer> PointInitializer = Get(PointsPin, Query);
	const TValue<FVoxelSeed> Seed = Get(SeedPin, Query);

	return VOXEL_ON_COMPLETE(PointInitializer, Seed)
	{
		const FVoxelPointRandom Random(Seed, STATIC_HASH("MakePoint"));

		const TSharedRef<FVoxelPointSet> Points = MakeVoxelShared<FVoxelPointSet>();
		Points->SetNum(PointInitializer->Points.Num());

		FVoxelPointIdBufferStorage Ids;
		FVoxelStaticMeshBufferStorage Meshes;
		FVoxelFloatBufferStorage PositionX;
		FVoxelFloatBufferStorage PositionY;
		FVoxelFloatBufferStorage PositionZ;
		FVoxelFloatBufferStorage RotationX;
		FVoxelFloatBufferStorage RotationY;
		FVoxelFloatBufferStorage RotationZ;
		FVoxelFloatBufferStorage RotationW;
		FVoxelFloatBufferStorage ScaleX;
		FVoxelFloatBufferStorage ScaleY;
		FVoxelFloatBufferStorage ScaleZ;
		FVoxelFloatBufferStorage NormalX;
		FVoxelFloatBufferStorage NormalY;
		FVoxelFloatBufferStorage NormalZ;

		for (int32 Index = 0; Index < PointInitializer->Points.Num(); Index++)
		{
			const FVoxelMakePointPoint& Point = PointInitializer->Points[Index];

			Ids.Add(Random.MakeId({}, Index));
			Meshes.Add(Point.RuntimeMesh);

			PositionX.Add(Point.Transform.GetLocation().X);
			PositionY.Add(Point.Transform.GetLocation().Y);
			PositionZ.Add(Point.Transform.GetLocation().Z);

			RotationX.Add(Point.Transform.GetRotation().X);
			RotationY.Add(Point.Transform.GetRotation().Y);
			RotationZ.Add(Point.Transform.GetRotation().Z);
			RotationW.Add(Point.Transform.GetRotation().W);

			ScaleX.Add(Point.Transform.GetScale3D().X);
			ScaleY.Add(Point.Transform.GetScale3D().Y);
			ScaleZ.Add(Point.Transform.GetScale3D().Z);

			NormalX.Add(Point.Normal.X);
			NormalY.Add(Point.Normal.Y);
			NormalZ.Add(Point.Normal.Z);
		}

		Points->Add(FVoxelPointAttributes::Id, FVoxelPointIdBuffer::Make(Ids));
		Points->Add(FVoxelPointAttributes::Position, FVoxelVectorBuffer::Make(PositionX, PositionY, PositionZ));
		Points->Add(FVoxelPointAttributes::Rotation, FVoxelQuaternionBuffer::Make(RotationX, RotationY, RotationZ, RotationW));
		Points->Add(FVoxelPointAttributes::Scale, FVoxelVectorBuffer::Make(ScaleX, ScaleY, ScaleZ));
		Points->Add(FVoxelPointAttributes::Normal, FVoxelVectorBuffer::Make(NormalX, NormalY, NormalZ));
		Points->Add(FVoxelPointAttributes::Mesh, FVoxelStaticMeshBuffer::Make(Meshes));

		return Points;
	};
}