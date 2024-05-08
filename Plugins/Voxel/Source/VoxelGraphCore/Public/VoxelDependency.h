// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class FVoxelDependency;
class FVoxelDependencyTracker;

DECLARE_VOXEL_MEMORY_STAT(VOXELGRAPHCORE_API, STAT_VoxelDependencies, "Dependencies");

class VOXELGRAPHCORE_API FVoxelDependencyInvalidationScope
{
public:
	FVoxelDependencyInvalidationScope();
	~FVoxelDependencyInvalidationScope();

private:
	TVoxelChunkedArray<TWeakPtr<FVoxelDependencyTracker>> Trackers;

	void Invalidate();

	friend FVoxelDependency;
};

class VOXELGRAPHCORE_API FVoxelDependency : public TSharedFromThis<FVoxelDependency>
{
public:
	const FName ClassName;
	const FName InstanceName;

	static TSharedRef<FVoxelDependency> Create(
		const FName ClassName,
		const FName InstanceName)
	{
		return TSharedRef<FVoxelDependency>(new FVoxelDependency(ClassName, InstanceName));
	}
	~FVoxelDependency() = default;
	UE_NONCOPYABLE(FVoxelDependency);

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelDependencies);

	int64 GetAllocatedSize() const
	{
		return TrackerRefs_RequiresLock.GetAllocatedSize();
	}

	struct FInvalidationParameters
	{
		TOptional<FVoxelBox> Bounds;
		TOptional<uint64> LessOrEqualTag;
	};
	void Invalidate(FInvalidationParameters Parameters = {});

private:
	FVoxelFastCriticalSection CriticalSection;

	struct FTrackerRef
	{
		TWeakPtr<FVoxelDependencyTracker> WeakTracker;

		bool bHasBounds = false;
		FVoxelBox Bounds;

		bool bHasTag = false;
		uint64 Tag = 0;
	};
	TVoxelChunkedSparseArray<FTrackerRef> TrackerRefs_RequiresLock;

	FVoxelDependency(
		const FName ClassName,
		const FName InstanceName);

	friend FVoxelDependencyTracker;
};

class VOXELGRAPHCORE_API FVoxelDependencyTracker : public TSharedFromThis<FVoxelDependencyTracker>
{
public:
	const FName Name;

	static TSharedRef<FVoxelDependencyTracker> Create(const FName Name)
	{
		return MakeVoxelShareable(new (GVoxelMemory) FVoxelDependencyTracker(Name));
	}
	~FVoxelDependencyTracker();
	UE_NONCOPYABLE(FVoxelDependencyTracker);

	VOXEL_COUNT_INSTANCES();

	FORCEINLINE bool IsInvalidated() const
	{
		return bIsInvalidated.Load();
	}
	void AddDependency(
		const TSharedRef<FVoxelDependency>& Dependency,
		const TOptional<FVoxelBox>& Bounds = {},
		const TOptional<uint64>& Tag = {});

	// Returns false if already invalidated
	bool TrySetOnInvalidated(TVoxelUniqueFunction<void()>&& NewOnInvalidated);

	template<typename T>
	void AddObjectToKeepAlive(const TSharedPtr<T>& ObjectToKeepAlive)
	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		ObjectsToKeepAlive.Add(MakeSharedVoidPtr(ObjectToKeepAlive));
	}

private:
	struct FDependencyRef
	{
		TWeakPtr<FVoxelDependency> WeakDependency;
		int32 Index = -1;
	};

	FVoxelFastCriticalSection CriticalSection;
	TVoxelAtomic<bool> bIsInvalidated;
	TVoxelUniqueFunction<void()> OnInvalidated;
	TVoxelChunkedArray<FDependencyRef> DependencyRefs;
	TVoxelArray<FSharedVoidPtr> ObjectsToKeepAlive;
	TVoxelSet<TWeakPtr<FVoxelDependency>> AddedDependencies;

	explicit FVoxelDependencyTracker(const FName& Name);

	void Unregister_RequiresLock();

	friend FVoxelDependency;
	friend FVoxelDependencyInvalidationScope;
};