// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointChunkRef.h"
#include "Point/VoxelPointSubsystem.h"
#include "VoxelQuery.h"
#include "VoxelRuntime.h"
#include "VoxelTaskGroup.h"
#include "VoxelDependency.h"
#include "VoxelRuntimeProvider.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, float, GVoxelPointCacheDuration, 1,
	"voxel.point.CacheDuration",
	"Duration, in seconds, for which to cache points");

class FVoxelPointChunkCache : public FVoxelSingleton
{
public:
	//~ Begin FVoxelSingleton Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();
		VOXEL_SCOPE_LOCK(CriticalSection);

		const double KillTime = FPlatformTime::Seconds() - GVoxelPointCacheDuration;
		for (auto It = ChunkRefToData_RequiresLock.CreateIterator(); It; ++It)
		{
			if (It.Value()->LastAccessTime < KillTime ||
				It.Value()->DependencyTracker->IsInvalidated())
			{
				It.RemoveCurrent();
			}
		}
	}
	//~ End FVoxelSingleton Interface

	TSharedPtr<const FVoxelPointSet> GetPoints(
		const FVoxelPointChunkRef& ChunkRef,
		FString* OutError)
	{
		VOXEL_FUNCTION_COUNTER();
		VOXEL_SCOPE_LOCK(CriticalSection);
		VOXEL_SCOPE_COUNTER_FORMAT("%s", *ChunkRef.ChunkProviderRef.NodePath.ToDebugString());

		if (const TSharedPtr<const FData> Data = ChunkRefToData_RequiresLock.FindRef(ChunkRef))
		{
			if (!Data->DependencyTracker->IsInvalidated())
			{
				Data->LastAccessTime = FPlatformTime::Seconds();
				if (OutError)
				{
					*OutError = Data->Error;
				}
				return Data->PointSet;
			}
		}
		ensure(IsInGameThread());

		UWorld* World = ChunkRef.GetWorld();
		if (!World)
		{
			if (OutError)
			{
				*OutError = "Invalid World";
			}
			return nullptr;
		}

		const TSharedRef<FVoxelRuntimeInfo> RuntimeInfo =
			FVoxelRuntimeInfoBase::MakeFromWorld(World)
			.EnableParallelTasks()
			.MakeRuntimeInfo();

		const TSharedRef<FData> Data = MakeVoxelShared<FData>();
		ChunkRefToData_RequiresLock.Add(ChunkRef, Data);

		const TSharedPtr<const FVoxelChunkedPointSet> ChunkedPointSet = ChunkRef.ChunkProviderRef.GetChunkedPointSet(&Data->Error);
		if (!ChunkedPointSet ||
			!ensure(ChunkedPointSet->GetChunkSize() == ChunkRef.ChunkSize))
		{
			return nullptr;
		}

		Data->DependencyTracker = FVoxelDependencyTracker::Create(STATIC_FNAME("FVoxelPointHandle"));

		bool bSuccess = true;
		TOptional<FVoxelPointSet> OptionalPoints = FVoxelTaskGroup::TryRunSynchronously(FVoxelQueryContext::Make(RuntimeInfo), [&]() -> TVoxelFutureValue<FVoxelPointSet>
		{
			const TVoxelFutureValue<FVoxelPointSet> FuturePoints = ChunkedPointSet->GetPoints(*Data->DependencyTracker, ChunkRef.ChunkMin);
			if (!FuturePoints.IsValid())
			{
				bSuccess = false;
				return FVoxelPointSet();
			}

			return FuturePoints;
		}, &Data->Error);

		if (!bSuccess ||
			!OptionalPoints.IsSet())
		{
			return nullptr;
		}

		Data->PointSet = MakeSharedCopy(MoveTemp(*OptionalPoints));
		return Data->PointSet;
	}

private:
	struct FData
	{
		mutable double LastAccessTime = FPlatformTime::Seconds();
		FString Error;
		TSharedPtr<const FVoxelPointSet> PointSet;
		TSharedPtr<FVoxelDependencyTracker> DependencyTracker;
	};

	FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FVoxelPointChunkRef, TSharedPtr<const FData>> ChunkRefToData_RequiresLock;
};

FVoxelPointChunkCache* GVoxelPointChunkCache = MakeVoxelSingleton(FVoxelPointChunkCache);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelRuntime> FVoxelPointChunkProviderRef::GetRuntime(FString* OutError) const
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	const UObject* RuntimeProviderObject = RuntimeProvider.Get();
	if (!RuntimeProviderObject)
	{
		if (OutError)
		{
			*OutError = "Invalid RuntimeProvider";
		}
		return {};
	}

	const IVoxelRuntimeProvider* RuntimeProviderInterface = Cast<IVoxelRuntimeProvider>(RuntimeProviderObject);
	if (!ensure(RuntimeProviderInterface))
	{
		if (OutError)
		{
			*OutError = "Invalid RuntimeProvider";
		}
		return {};
	}

	const TSharedPtr<FVoxelRuntime> Runtime = RuntimeProviderInterface->GetRuntime();
	if (!Runtime)
	{
		if (OutError)
		{
			*OutError = "Runtime is destroyed";
		}
		return {};
	}

	return Runtime;
}

TSharedPtr<const FVoxelChunkedPointSet> FVoxelPointChunkProviderRef::GetChunkedPointSet(FString* OutError) const
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FVoxelRuntime> Runtime = GetRuntime(OutError);
	if (!Runtime)
	{
		if (OutError)
		{
			*OutError = "Invalid Runtime";
		}
		return nullptr;
	}

	const TSharedPtr<FVoxelPointSubsystem> Subsystem = Runtime->FindSubsystem<FVoxelPointSubsystem>();
	if (!ensure(Subsystem))
	{
		return nullptr;
	}

	TSharedPtr<const FVoxelChunkedPointSet> ChunkedPointSet;
	{
		VOXEL_SCOPE_LOCK(Subsystem->CriticalSection);
		ChunkedPointSet = Subsystem->ChunkProviderToChunkedPointSet_RequiresLock.FindRef(*this);
	}

	if (!ChunkedPointSet)
	{
		if (OutError)
		{
			*OutError = "Could not find matching ChunkedPointSet";
		}
		return nullptr;
	}

	return ChunkedPointSet;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<const FVoxelPointSet> FVoxelPointChunkRef::GetPoints(FString* OutError) const
{
	return GVoxelPointChunkCache->GetPoints(*this, OutError);
}

bool FVoxelPointChunkRef::NetSerialize(FArchive& Ar, UPackageMap& Map)
{
	VOXEL_FUNCTION_COUNTER();

	if (Ar.IsSaving())
	{
		ensure(IsValid());

		UObject* Object = ConstCast(ChunkProviderRef.RuntimeProvider.Get());
		ensure(Object);

		if (!ensure(Map.SerializeObject(Ar, UObject::StaticClass(), Object)))
		{
			return false;
		}

		if (!ensure(ChunkProviderRef.NodePath.NetSerialize(Ar, Map)))
		{
			return false;
		}

		Ar << ChunkMin;
		Ar << ChunkSize;
		return true;
	}
	else if (Ar.IsLoading())
	{
		UObject* Object = nullptr;
		if (!ensure(Map.SerializeObject(Ar, UObject::StaticClass(), Object)) ||
			!ensure(Object))
		{
			return false;
		}

		ChunkProviderRef.RuntimeProvider = Object;

		if (!ensure(ChunkProviderRef.NodePath.NetSerialize(Ar, Map)))
		{
			return false;
		}

		Ar << ChunkMin;
		Ar << ChunkSize;
		return true;
	}
	else
	{
		ensure(false);
		return false;
	}
}