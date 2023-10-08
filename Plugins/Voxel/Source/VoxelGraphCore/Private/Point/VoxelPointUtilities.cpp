// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointUtilities.h"
#include "VoxelBufferUtilities.h"

void FVoxelPointUtilities::GetMeshBounds(
	const FVoxelStaticMeshBuffer& MeshBuffer,
	const FVoxelVectorBuffer& PositionBuffer,
	const FVoxelQuaternionBuffer& RotationBuffer,
	const FVoxelVectorBuffer& ScaleBuffer,
	FVoxelVectorBuffer& OutBoundsMin,
	FVoxelVectorBuffer& OutBoundsMax)
{
	const FVoxelBufferAccessor BufferAccessor(
		MeshBuffer,
		PositionBuffer,
		RotationBuffer,
		ScaleBuffer);
	if (!ensure(BufferAccessor.IsValid()))
	{
		return;
	}

	VOXEL_FUNCTION_COUNTER_NUM(BufferAccessor.Num(), 1024);

	const TSharedRef<FVoxelFloatBufferStorage> MinX = MakeVoxelShared<FVoxelFloatBufferStorage>();
	const TSharedRef<FVoxelFloatBufferStorage> MinY = MakeVoxelShared<FVoxelFloatBufferStorage>();
	const TSharedRef<FVoxelFloatBufferStorage> MinZ = MakeVoxelShared<FVoxelFloatBufferStorage>();
	const TSharedRef<FVoxelFloatBufferStorage> MaxX = MakeVoxelShared<FVoxelFloatBufferStorage>();
	const TSharedRef<FVoxelFloatBufferStorage> MaxY = MakeVoxelShared<FVoxelFloatBufferStorage>();
	const TSharedRef<FVoxelFloatBufferStorage> MaxZ = MakeVoxelShared<FVoxelFloatBufferStorage>();
	MinX->Allocate(BufferAccessor.Num());
	MinY->Allocate(BufferAccessor.Num());
	MinZ->Allocate(BufferAccessor.Num());
	MaxX->Allocate(BufferAccessor.Num());
	MaxY->Allocate(BufferAccessor.Num());
	MaxZ->Allocate(BufferAccessor.Num());
	{
		VOXEL_SCOPE_COUNTER_FORMAT("Build Corners Num=%d", BufferAccessor.Num());

		struct FBounds
		{
			FVector3f Min;
			FVector3f Max;
		};
		TVoxelAddOnlyMap<FVoxelStaticMesh, FBounds> MeshToBounds;
		MeshToBounds.Reserve(16);

		for (int32 Index = 0; Index < BufferAccessor.Num(); Index++)
		{
			const FVoxelStaticMesh Mesh = MeshBuffer[Index];
			if (const FBounds* BoundsPtr = MeshToBounds.Find(Mesh))
			{
				(*MinX)[Index] = BoundsPtr->Min.X;
				(*MinY)[Index] = BoundsPtr->Min.Y;
				(*MinZ)[Index] = BoundsPtr->Min.Z;
				(*MaxX)[Index] = BoundsPtr->Max.X;
				(*MaxY)[Index] = BoundsPtr->Max.Y;
				(*MaxZ)[Index] = BoundsPtr->Max.Z;
			}
			else
			{
				const FBox Box = Mesh.GetMeshInfo().MeshBox;

				FBounds Bounds;
				Bounds.Min = FVector3f(Box.Min);
				Bounds.Max = FVector3f(Box.Max);
				MeshToBounds.Add_CheckNew(Mesh, Bounds);

				(*MinX)[Index] = Bounds.Min.X;
				(*MinY)[Index] = Bounds.Min.Y;
				(*MinZ)[Index] = Bounds.Min.Z;
				(*MaxX)[Index] = Bounds.Max.X;
				(*MaxY)[Index] = Bounds.Max.Y;
				(*MaxZ)[Index] = Bounds.Max.Z;
			}
		}
	}

	FVoxelVectorBuffer MinMinMin; MinMinMin.X.SetStorage(MinX); MinMinMin.Y.SetStorage(MinY); MinMinMin.Z.SetStorage(MinZ);
	FVoxelVectorBuffer MaxMinMin; MaxMinMin.X.SetStorage(MaxX); MaxMinMin.Y.SetStorage(MinY); MaxMinMin.Z.SetStorage(MinZ);
	FVoxelVectorBuffer MinMaxMin; MinMaxMin.X.SetStorage(MinX); MinMaxMin.Y.SetStorage(MaxY); MinMaxMin.Z.SetStorage(MinZ);
	FVoxelVectorBuffer MaxMaxMin; MaxMaxMin.X.SetStorage(MaxX); MaxMaxMin.Y.SetStorage(MaxY); MaxMaxMin.Z.SetStorage(MinZ);
	FVoxelVectorBuffer MinMinMax; MinMinMax.X.SetStorage(MinX); MinMinMax.Y.SetStorage(MinY); MinMinMax.Z.SetStorage(MaxZ);
	FVoxelVectorBuffer MaxMinMax; MaxMinMax.X.SetStorage(MaxX); MaxMinMax.Y.SetStorage(MinY); MaxMinMax.Z.SetStorage(MaxZ);
	FVoxelVectorBuffer MinMaxMax; MinMaxMax.X.SetStorage(MinX); MinMaxMax.Y.SetStorage(MaxY); MinMaxMax.Z.SetStorage(MaxZ);
	FVoxelVectorBuffer MaxMaxMax; MaxMaxMax.X.SetStorage(MaxX); MaxMaxMax.Y.SetStorage(MaxY); MaxMaxMax.Z.SetStorage(MaxZ);

	MinMinMin = FVoxelBufferUtilities::ApplyTransform(MinMinMin, PositionBuffer, RotationBuffer, ScaleBuffer);
	MaxMinMin = FVoxelBufferUtilities::ApplyTransform(MaxMinMin, PositionBuffer, RotationBuffer, ScaleBuffer);
	MinMaxMin = FVoxelBufferUtilities::ApplyTransform(MinMaxMin, PositionBuffer, RotationBuffer, ScaleBuffer);
	MaxMaxMin = FVoxelBufferUtilities::ApplyTransform(MaxMaxMin, PositionBuffer, RotationBuffer, ScaleBuffer);
	MinMinMax = FVoxelBufferUtilities::ApplyTransform(MinMinMax, PositionBuffer, RotationBuffer, ScaleBuffer);
	MaxMinMax = FVoxelBufferUtilities::ApplyTransform(MaxMinMax, PositionBuffer, RotationBuffer, ScaleBuffer);
	MinMaxMax = FVoxelBufferUtilities::ApplyTransform(MinMaxMax, PositionBuffer, RotationBuffer, ScaleBuffer);
	MaxMaxMax = FVoxelBufferUtilities::ApplyTransform(MaxMaxMax, PositionBuffer, RotationBuffer, ScaleBuffer);

	OutBoundsMin = FVoxelBufferUtilities::Min8(MinMinMin, MaxMinMin, MinMaxMin, MaxMaxMin, MinMinMax, MaxMinMax, MinMaxMax, MaxMaxMax);
	OutBoundsMax = FVoxelBufferUtilities::Max8(MinMinMin, MaxMinMin, MinMaxMin, MaxMaxMin, MinMinMax, MaxMinMax, MinMaxMax, MaxMaxMax);
}