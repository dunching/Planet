// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFutureValue.h"

struct FVoxelNode;
class FVoxelTaskStat;
class FVoxelTaskGroup;
class FVoxelTransformRef;
class FVoxelGraphExecutor;

extern VOXELGRAPHCORE_API bool GVoxelBypassTaskQueue;

VOXELGRAPHCORE_API bool ShouldExitVoxelTask();
VOXELGRAPHCORE_API bool ShouldRunVoxelTaskInParallel();

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct VOXELGRAPHCORE_API FVoxelTaskPriority
{
public:
	FVoxelTaskPriority() = default;

	FORCEINLINE static FVoxelTaskPriority MakeTop()
	{
		return FVoxelTaskPriority();
	}
	FORCEINLINE static FVoxelTaskPriority MakeBounds(
		const FVoxelBox& Bounds,
		const double Offset,
		const FObjectKey World,
		const FVoxelTransformRef& LocalToWorld)
	{
		return FVoxelTaskPriority(true, Bounds, Offset, GetPosition(World, LocalToWorld));
	}

	FORCEINLINE double GetPriority() const
	{
		if (!bHasBounds)
		{
			return -1;
		}
		checkVoxelSlow(Position.IsValid());

		const double DistanceSquared = Bounds.ComputeSquaredDistanceFromBoxToPoint(*Position.Get());
		// Keep the sign of Offset
		return DistanceSquared + Offset * FMath::Abs(Offset);
	}

private:
	bool bHasBounds = false;
	FVoxelBox Bounds;
	double Offset = 0;
	TSharedPtr<const FVector> Position;

	FORCEINLINE FVoxelTaskPriority(
		const bool bHasBounds,
		const FVoxelBox& Bounds,
		const double PriorityOffset,
		const TSharedPtr<const FVector>& Position)
		: bHasBounds(bHasBounds)
		, Bounds(Bounds)
		, Offset(PriorityOffset)
		, Position(Position)
	{
	}

	static TSharedRef<const FVector> GetPosition(
		const FObjectKey World,
		const FVoxelTransformRef& LocalToWorld);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelTaskReferencer
{
public:
	const FName Name;

	explicit FVoxelTaskReferencer(FName Name);

	VOXEL_COUNT_INSTANCES();

	void AddRef(const TSharedRef<const FVirtualDestructor>& Object);
	void AddExecutor(const TSharedRef<const FVoxelGraphExecutor>& GraphExecutor);
	bool IsReferenced(const FVirtualDestructor* Object) const;
	bool IsReferenced(const FVoxelNode* Node) const;

	static FVoxelTaskReferencer& Get();

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlySet<TSharedPtr<const FVirtualDestructor>> Objects;
	TVoxelAddOnlySet<TSharedPtr<const FVoxelGraphExecutor>> GraphExecutors;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_SPARSE_INDEX(FVoxelPendingTaskId);

struct VOXELGRAPHCORE_API FVoxelTask
{
	FVoxelTaskGroup& Group;
	const FName Name;
	const EVoxelTaskThread Thread;
	const TVoxelUniqueFunction<void()> Lambda;

	FThreadSafeCounter NumDependencies;
	FVoxelPendingTaskId PendingTaskId;

	FVoxelTask(
		FVoxelTaskGroup& Group,
		const FName Name,
		const EVoxelTaskThread Thread,
		TVoxelUniqueFunction<void()>&& Lambda)
		: Group(Group)
		, Name(Name)
		, Thread(Thread)
		, Lambda(MoveTemp(Lambda))
	{
	}
	UE_NONCOPYABLE(FVoxelTask);

	VOXEL_COUNT_INSTANCES();

	void Execute() const;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelTaskFactory
{
public:
	FVoxelTaskFactory() = default;

	FORCEINLINE FVoxelTaskFactory& NeverBypassQueue()
	{
		bPrivateNeverBypassQueue = true;
		return *this;
	}
	FORCEINLINE FVoxelTaskFactory& Thread(const EVoxelTaskThread NewThread)
	{
		PrivateThread = NewThread;
		return *this;
	}
	FORCEINLINE FVoxelTaskFactory& Dependency(const FVoxelFutureValue& Dependency)
	{
		ensureVoxelSlow(DependenciesView.Num() == 0);
		DependenciesView = MakeVoxelArrayView(Dependency);
		return *this;
	}
	FORCEINLINE FVoxelTaskFactory& Dependencies(const TConstVoxelArrayView<FVoxelFutureValue> Dependencies)
	{
		ensureVoxelSlow(DependenciesView.Num() == 0);
		DependenciesView = Dependencies;
		return *this;
	}
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelFutureValue>::Value>::Type>
	FORCEINLINE FVoxelTaskFactory& Dependencies(const TVoxelArray<T>& Dependencies)
	{
		ensureVoxelSlow(DependenciesView.Num() == 0);
		DependenciesView = ReinterpretCastVoxelArrayView<FVoxelFutureValue>(MakeVoxelArrayView(Dependencies));
		return *this;
	}
	template<typename... ArgTypes, typename = typename TEnableIf<TAnd<TIsDerivedFrom<ArgTypes, FVoxelFutureValue>...>::Value>::Type>
	FORCEINLINE FVoxelTaskFactory& Dependencies(const ArgTypes&... Args)
	{
		ensureVoxelSlow(InlineDependencies.Num() == 0);
		VOXEL_FOLD_EXPRESSION(InlineDependencies.Add(Args));
		return Dependencies(InlineDependencies);
	}

	void Execute(TVoxelUniqueFunction<void()>&& Lambda);
	FVoxelFutureValue Execute(const FVoxelPinType& Type, TVoxelUniqueFunction<FVoxelFutureValue()>&& Lambda);

	template<typename T>
	TVoxelFutureValue<T> Execute(TVoxelUniqueFunction<TVoxelFutureValue<T>()>&& Lambda)
	{
		return this->Execute<T>(FVoxelPinType::Make<T>(), MoveTemp(Lambda));
	}
	template<typename T>
	TVoxelFutureValue<T> Execute(const FVoxelPinType& Type, TVoxelUniqueFunction<TVoxelFutureValue<T>()>&& Lambda)
	{
		checkVoxelSlow(Type.CanBeCastedTo<T>());
		return TVoxelFutureValue<T>(this->Execute(Type, ReinterpretCastRef<TVoxelUniqueFunction<FVoxelFutureValue()>>(MoveTemp(Lambda))));
	}

private:
	FName PrivateName;
	bool bPrivateNeverBypassQueue = false;
	EVoxelTaskThread PrivateThread = EVoxelTaskThread::AsyncThread;
	TConstVoxelArrayView<FVoxelFutureValue> DependenciesView;
	TVoxelArray<FVoxelFutureValue, TVoxelInlineAllocator<8>> InlineDependencies;

	friend FVoxelTaskFactory MakeVoxelTask(FName);
};

FORCEINLINE FVoxelTaskFactory MakeVoxelTask(const FName Name = {})
{
	FVoxelTaskFactory Factory;
	Factory.PrivateName = Name;
	return Factory;
}