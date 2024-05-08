// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

struct IVoxelNodeInterface;

#if WITH_EDITOR
extern VOXELGRAPHCORE_API bool GVoxelEnableNodeStats;

struct IVoxelNodeStatProvider
{
	IVoxelNodeStatProvider() = default;
	virtual ~IVoxelNodeStatProvider() = default;

	virtual void ClearStats() = 0;
	virtual FText GetToolTip(const UEdGraphNode& Node) = 0;
	virtual FText GetText(const UEdGraphNode& Node) = 0;
};
extern VOXELGRAPHCORE_API TArray<IVoxelNodeStatProvider*> GVoxelNodeStatProviders;
#endif

class VOXELGRAPHCORE_API FVoxelNodeStatScope
{
public:
	FORCEINLINE FVoxelNodeStatScope(const IVoxelNodeInterface& InNode, const int64 InCount)
	{
#if WITH_EDITOR
		if (!GVoxelEnableNodeStats)
		{
			return;
		}

		Node = &InNode;
		Count = InCount;
		StartTime = FPlatformTime::Seconds();
#endif
	}
	FORCEINLINE ~FVoxelNodeStatScope()
	{
#if WITH_EDITOR
		if (IsEnabled())
		{
			RecordStats(FPlatformTime::Seconds() - StartTime);
		}
#endif
	}

	FORCEINLINE bool IsEnabled() const
	{
#if WITH_EDITOR
		return Node != nullptr;
#else
		return false;
#endif
	}
	FORCEINLINE void SetCount(const int32 NewCount)
	{
#if WITH_EDITOR
		Count = NewCount;
#endif
	}

private:
#if WITH_EDITOR
	const IVoxelNodeInterface* Node = nullptr;
	int64 Count = 0;
	double StartTime = 0;

	void RecordStats(const double Duration) const;
#endif
};