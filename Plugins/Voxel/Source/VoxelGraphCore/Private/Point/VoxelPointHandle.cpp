// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointHandle.h"
#include "Point/VoxelPointSet.h"
#include "VoxelRuntime.h"
#include "VoxelExecNode.h"

TSharedPtr<FVoxelRuntime> FVoxelPointHandle::GetRuntime(FString* OutError) const
{
	return ChunkRef.ChunkProviderRef.GetRuntime(OutError);
}

bool FVoxelPointHandle::GetAttributes(
	TVoxelMap<FName, FVoxelPinValue>& InOutAttributes,
	FString* OutError) const
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<const FVoxelPointSet> PointSet = ChunkRef.GetPoints(OutError);
	if (!PointSet)
	{
		ensure(!OutError || !OutError->IsEmpty());
		return false;
	}

	const int32 Index = PointSet->GetPointIdToIndex().Find(PointId);
	if (Index == -1)
	{
		if (OutError)
		{
			*OutError = "Failed to find matching point id";
		}
		return false;
	}

	for (auto& It : InOutAttributes)
	{
		const TSharedPtr<const FVoxelBuffer> Buffer = PointSet->Find(It.Key);
		if (!Buffer)
		{
			continue;
		}

		const FVoxelRuntimePinValue RuntimeValue = Buffer->GetGeneric(Index);
		const FVoxelPinValue ExposedValue = FVoxelPinType::MakeExposedValue(RuntimeValue, false);
		if (!ensure(ExposedValue.IsValid()))
		{
			continue;
		}

		It.Value = ExposedValue;
	}
	return true;
}

bool FVoxelPointHandle::Serialize(FArchive& Ar)
{
	VOXEL_FUNCTION_COUNTER();

	Ar << ChunkRef;
	Ar << PointId.PointId;
	return true;
}

bool FVoxelPointHandle::Identical(const FVoxelPointHandle* Other, uint32 PortFlags) const
{
	return *this == *Other;
}

bool FVoxelPointHandle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!ensure(Map))
	{
		bOutSuccess = false;
		return true;
	}

	bOutSuccess = NetSerializeImpl(Ar, *Map);
	return true;
}

bool FVoxelPointHandle::NetSerializeImpl(FArchive& Ar, UPackageMap& Map)
{
	VOXEL_FUNCTION_COUNTER();

	if (Ar.IsSaving())
	{
		uint8 IsValidByte = IsValid();
		Ar.SerializeBits(&IsValidByte, 1);

		if (!IsValidByte)
		{
			return true;
		}

		if (!ensure(ChunkRef.NetSerialize(Ar, Map)))
		{
			return false;
		}

		Ar << PointId.PointId;
		return true;
	}
	else if (Ar.IsLoading())
	{
		uint8 IsValidByte = 0;
		Ar.SerializeBits(&IsValidByte, 1);

		if (!IsValidByte)
		{
			*this = {};
			return true;
		}

		if (!ensure(ChunkRef.NetSerialize(Ar, Map)))
		{
			return false;
		}

		Ar << PointId.PointId;
		return true;
	}
	else
	{
		ensure(false);
		return false;
	}
}