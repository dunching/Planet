// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Sculpt/VoxelSculptStorage.h"
#include "Sculpt/VoxelSculptStorageData.h"

void UVoxelSculptStorage::Serialize(FArchive& Ar)
{
	VOXEL_FUNCTION_COUNTER();

	Super::Serialize(Ar);

	if (IsReloadActive())
	{
		return;
	}

	if (!Data)
	{
		Data = MakeVoxelShared<FVoxelSculptStorageData>(GetFName());
	}

	FVoxelObjectUtilities::SerializeBulkData(this, BulkData, Ar, [&](FArchive& BulkDataAr)
	{
		Data->Serialize(BulkDataAr);
	});
}

void UVoxelSculptStorage::ClearData()
{
	VOXEL_FUNCTION_COUNTER();

	if (!Data)
	{
		return;
	}

	Data->ClearData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelSculptStorageData> UVoxelSculptStorage::GetData()
{
	if (!Data)
	{
		Data = MakeVoxelShared<FVoxelSculptStorageData>(GetFName());
	}
	return Data.ToSharedRef();
}