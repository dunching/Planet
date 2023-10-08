// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"

class VOXELGRAPHCORE_API FVoxelQueryCache
{
public:
	struct FEntry
	{
		FVoxelFastCriticalSection CriticalSection;
		FVoxelFutureValue Value;
	};

	FVoxelQueryCache()
	{
		Entries.Reserve(128);
	}

	FEntry& FindOrAddEntry(const FVoxelPinRuntimeId PinId)
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		TSharedPtr<FEntry>& Entry = Entries.FindOrAdd(PinId);
		if (!Entry)
		{
			Entry = MakeVoxelShared<FEntry>();
		}
		return *Entry;
	}

private:
	FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlyMap<FVoxelPinRuntimeId, TSharedPtr<FEntry>> Entries;
};