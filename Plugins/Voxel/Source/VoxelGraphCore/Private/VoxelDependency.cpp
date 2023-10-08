// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelDependency.h"
#include "VoxelTask.h"

DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelDependencies);
DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelDependencyTracker);

thread_local FVoxelDependencyInvalidationScope* GVoxelDependencyInvalidationScope = nullptr;

FVoxelDependencyInvalidationScope::FVoxelDependencyInvalidationScope()
{
	if (!GVoxelDependencyInvalidationScope)
	{
		GVoxelDependencyInvalidationScope = this;
	}
}

FVoxelDependencyInvalidationScope::~FVoxelDependencyInvalidationScope()
{
	if (Trackers.Num() > 0)
	{
		Invalidate();
	}

	if (GVoxelDependencyInvalidationScope == this)
	{
		GVoxelDependencyInvalidationScope = nullptr;
	}
}

void FVoxelDependencyInvalidationScope::Invalidate()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(GVoxelDependencyInvalidationScope == this);

	if (IsInGameThread() &&
		!GVoxelBypassTaskQueue)
	{
		AsyncVoxelTask([Trackers = MakeUniqueCopy(MoveTemp(Trackers))]
		{
			FVoxelDependencyInvalidationScope Invalidator;

			ensure(GVoxelDependencyInvalidationScope->Trackers.Num() == 0);
			GVoxelDependencyInvalidationScope->Trackers = MoveTemp(*Trackers);
		});
		return;
	}

	while (Trackers.Num() > 0)
	{
		TVoxelArray<TVoxelUniqueFunction<void()>> OnInvalidatedArray;
		OnInvalidatedArray.Reserve(Trackers.Num());

		for (const TWeakPtr<FVoxelDependencyTracker>& WeakTracker : Trackers)
		{
			const TSharedPtr<FVoxelDependencyTracker> Tracker = WeakTracker.Pin();
			if (!Tracker ||
				Tracker->IsInvalidated())
			{
				// Skip lock
				continue;
			}

			VOXEL_SCOPE_LOCK(Tracker->CriticalSection);

			if (Tracker->IsInvalidated())
			{
				continue;
			}

			Tracker->bIsInvalidated.Store(true);
			Tracker->Unregister_RequiresLock();

			if (Tracker->OnInvalidated)
			{
				OnInvalidatedArray.Add(MoveTemp(Tracker->OnInvalidated));
			}
		}
		Trackers.Empty();

		// This might add new trackers to Trackers
		VOXEL_SCOPE_COUNTER("OnInvalidated");
		for (const TVoxelUniqueFunction<void()>& OnInvalidated : OnInvalidatedArray)
		{
			OnInvalidated();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelDependency::Invalidate(const FInvalidationParameters Parameters)
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelDependencyInvalidationScope LocalScope;
	FVoxelDependencyInvalidationScope& RootScope = *GVoxelDependencyInvalidationScope;

	const bool bCheckBounds = Parameters.Bounds.IsSet();
	const FVoxelBox Bounds = Parameters.Bounds.Get({});

	const bool bCheckTag = Parameters.LessOrEqualTag.IsSet();
	const uint64 LessOrEqualTag = Parameters.LessOrEqualTag.Get({});

	VOXEL_SCOPE_LOCK(CriticalSection);

	if (TrackerRefs_RequiresLock.Num() > 0)
	{
		if (Parameters.Bounds.IsSet())
		{
			LOG_VOXEL(Verbose, "Invalidating %s %s Bounds=%s",
				*ClassName.ToString(),
				*InstanceName.ToString(),
				*Parameters.Bounds->ToString());
		}
		else
		{
			LOG_VOXEL(Verbose, "Invalidating %s %s",
				*ClassName.ToString(),
				*InstanceName.ToString());
		}
	}

	TrackerRefs_RequiresLock.Foreach([&](const FTrackerRef& TrackerRef)
	{
		if (bCheckBounds &&
			TrackerRef.bHasBounds &&
			!Bounds.Intersect(TrackerRef.Bounds))
		{
			return;
		}

		if (bCheckTag &&
			TrackerRef.bHasTag &&
			!(LessOrEqualTag <= TrackerRef.Tag))
		{
			return;
		}

		RootScope.Trackers.Add(TrackerRef.WeakTracker);
	});
}

FVoxelDependency::FVoxelDependency(const FName ClassName, const FName InstanceName)
	: ClassName(ClassName)
	, InstanceName(InstanceName)
{
	VOXEL_FUNCTION_COUNTER();
	TrackerRefs_RequiresLock.Reserve(8192);
	UpdateStats();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelDependencyTracker::~FVoxelDependencyTracker()
{
	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(!IsInvalidated() || DependencyRefs.Num() == 0);
	Unregister_RequiresLock();
}

void FVoxelDependencyTracker::AddDependency(
	const TSharedRef<FVoxelDependency>& Dependency,
	const TOptional<FVoxelBox>& Bounds,
	const TOptional<uint64>& Tag)
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelDependency::FTrackerRef TrackerRef;
	{
		TrackerRef.WeakTracker = AsWeak();

		if (Bounds.IsSet())
		{
			TrackerRef.bHasBounds = true;
			TrackerRef.Bounds = Bounds.GetValue();
		}

		if (Tag.IsSet())
		{
			TrackerRef.bHasTag = true;
			TrackerRef.Tag = Tag.GetValue();
		}
	}

	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(!OnInvalidated);

	if (IsInvalidated())
	{
		// Invalidated while still computing
		// No need to register the dependency
		return;
	}

	if (AddedDependencies.Contains(Dependency))
	{
		// Already added
		return;
	}
	AddedDependencies.Add(Dependency);

	int32 Index;
	{
		VOXEL_SCOPE_LOCK(Dependency->CriticalSection);
		Index = Dependency->TrackerRefs_RequiresLock.Add(TrackerRef);
	}
	Dependency->UpdateStats();

	FDependencyRef& DependencyRef = DependencyRefs.Emplace_GetRef();
	DependencyRef.WeakDependency = Dependency;
	DependencyRef.Index = Index;
}

bool FVoxelDependencyTracker::TrySetOnInvalidated(TVoxelUniqueFunction<void()>&& NewOnInvalidated)
{
	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(!OnInvalidated);

	if (IsInvalidated())
	{
		return false;
	}

	OnInvalidated = MoveTemp(NewOnInvalidated);
	return true;
}

FVoxelDependencyTracker::FVoxelDependencyTracker(const FName& Name)
	: Name(Name)
{
	DependencyRefs.Reserve(16);
}

void FVoxelDependencyTracker::Unregister_RequiresLock()
{
	VOXEL_FUNCTION_COUNTER();
	checkVoxelSlow(CriticalSection.IsLocked());

	for (const FDependencyRef& DependencyRef : DependencyRefs)
	{
		const TSharedPtr<FVoxelDependency> Dependency = DependencyRef.WeakDependency.Pin();
		if (!Dependency)
		{
			continue;
		}

		VOXEL_SCOPE_LOCK(Dependency->CriticalSection);

		checkVoxelSlow(GetWeakPtrObject_Unsafe(Dependency->TrackerRefs_RequiresLock[DependencyRef.Index].WeakTracker) == this);
		Dependency->TrackerRefs_RequiresLock.RemoveAt(DependencyRef.Index);
	}
	DependencyRefs.Empty();
}