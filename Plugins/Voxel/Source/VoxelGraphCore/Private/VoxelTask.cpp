// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelTask.h"
#include "VoxelBuffer.h"
#include "VoxelTaskGroup.h"
#include "VoxelGraphExecutor.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelBypassTaskQueue, false,
	"voxel.BypassTaskQueue",
	"If true, will bypass task queue whenever possible, making debugging easier");

VOXEL_RUN_ON_STARTUP_GAME(InitializeVoxelBypassTaskQueue)
{
	if (FParse::Param(FCommandLine::Get(), TEXT("VoxelBypassTaskQueue")))
	{
		GVoxelBypassTaskQueue = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool ShouldExitVoxelTask()
{
	return FVoxelTaskGroup::Get().ShouldExit();
}

bool ShouldRunVoxelTaskInParallel()
{
	if (IsInGameThreadFast())
	{
		return true;
	}

	const FVoxelTaskGroup* Group = FVoxelTaskGroup::TryGet();
	if (!ensureVoxelSlow(Group))
	{
		return false;
	}
	return Group->RuntimeInfo->ShouldRunTasksInParallel();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelTaskPriorityTicker : public FVoxelSingleton
{
public:
	FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<FObjectKey, TVoxelMap<FVoxelTransformRef, TWeakPtr<FVector>>> WorldToLocalToWorldToPosition;

	//~ Begin FVoxelSingleton Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();
		VOXEL_SCOPE_LOCK(CriticalSection);

		for (auto WorldIt = WorldToLocalToWorldToPosition.CreateIterator(); WorldIt; ++WorldIt)
		{
			const UWorld* World = CastChecked<UWorld>(WorldIt.Key().ResolveObjectPtr(), ECastCheckedType::NullAllowed);
			if (!World)
			{
				WorldIt.RemoveCurrent();
				continue;
			}

			FVector CameraPosition = FVector::ZeroVector;
			FVoxelGameUtilities::GetCameraView(World, CameraPosition);

			for (auto It = WorldIt.Value().CreateIterator(); It; ++It)
			{
				const TSharedPtr<FVector> PositionRef = It.Value().Pin();
				if (!PositionRef)
				{
					It.RemoveCurrent();
					continue;
				}

				const FMatrix LocalToWorld = It.Key().Get_NoDependency();
				*PositionRef = LocalToWorld.InverseTransformPosition(CameraPosition);
			}
		}
	}
	//~ End FVoxelSingleton Interface
};
FVoxelTaskPriorityTicker* GVoxelTaskPriorityTicker = MakeVoxelSingleton(FVoxelTaskPriorityTicker);

TSharedRef<const FVector> FVoxelTaskPriority::GetPosition(
	const FObjectKey World,
	const FVoxelTransformRef& LocalToWorld)
{
	VOXEL_SCOPE_LOCK(GVoxelTaskPriorityTicker->CriticalSection);

	TVoxelMap<FVoxelTransformRef, TWeakPtr<FVector>>& LocalToWorldToPosition = GVoxelTaskPriorityTicker->WorldToLocalToWorldToPosition.FindOrAdd(World);
	TWeakPtr<FVector>& WeakPosition = LocalToWorldToPosition.FindOrAdd(LocalToWorld);

	TSharedPtr<FVector> Position = WeakPosition.Pin();
	if (!Position)
	{
		Position = MakeVoxelShared<FVector>(FVector::ZAxisVector);
		WeakPosition = Position;
	}
	return Position.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelTaskReferencer);

FVoxelTaskReferencer::FVoxelTaskReferencer(const FName Name)
	: Name(Name)
{
	Objects.Reserve(4);
	GraphExecutors.Reserve(4);
}

void FVoxelTaskReferencer::AddRef(const TSharedRef<const FVirtualDestructor>& Object)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	Objects.Add(Object);
}

void FVoxelTaskReferencer::AddExecutor(const TSharedRef<const FVoxelGraphExecutor>& GraphExecutor)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	GraphExecutors.Add(GraphExecutor);
}

bool FVoxelTaskReferencer::IsReferenced(const FVirtualDestructor* Object) const
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	if (Objects.Contains(GetTypeHash(Object), [&](const TSharedPtr<const FVirtualDestructor>& OtherObject)
		{
			return OtherObject.Get() == Object;
		}))
	{
		return true;
	}

	for (const TSharedPtr<const FVoxelGraphExecutor>& GraphExecutor : GraphExecutors)
	{
		if (GraphExecutor->Nodes.Contains(GetTypeHash(Object), [&](const TSharedPtr<const FVoxelNode>& Node)
			{
				return Node.Get() == Object;
			}))
		{
			return true;
		}
	}

	return false;
}

bool FVoxelTaskReferencer::IsReferenced(const FVoxelNode* Node) const
{
	return
		Node->GetNodeRuntime().IsCallNode() ||
		IsReferenced(static_cast<const FVirtualDestructor*>(Node));
}

FVoxelTaskReferencer& FVoxelTaskReferencer::Get()
{
	return FVoxelTaskGroup::Get().GetReferencer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelTask);

void FVoxelTask::Execute() const
{
	checkVoxelSlow(Thread != EVoxelTaskThread::GameThread || IsInGameThread());
	checkVoxelSlow(Thread != EVoxelTaskThread::RenderThread || IsInRenderingThread());
	checkVoxelSlow(NumDependencies.GetValue() == 0);
	checkVoxelSlow(Group.RuntimeInfo->NumActiveTasks.GetValue() > 0);

	if (Name.IsNone())
	{
		Lambda();
	}
	else
	{
		VOXEL_SCOPE_COUNTER_FNAME(Name);
		Lambda();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTaskFactory::Execute(TVoxelUniqueFunction<void()>&& Lambda)
{
	TVoxelUniquePtr<FVoxelTask> TaskPtr = MakeVoxelUnique<FVoxelTask>(
		FVoxelTaskGroup::Get(),
		PrivateName,
		PrivateThread,
		MoveTemp(Lambda));

	FVoxelTask& Task = *TaskPtr;

	TVoxelArray<const FVoxelFutureValue*, TVoxelInlineAllocator<32>> DependenciesToProcess;
	for (const FVoxelFutureValue& Dependency : DependenciesView)
	{
		if (!ensureMsgf(Dependency.IsValid(), TEXT("Invalid dependency passed to %s"), *PrivateName.ToString()))
		{
			continue;
		}

		if (!Dependency.IsComplete())
		{
			DependenciesToProcess.Add(&Dependency);
		}
	}

	if (DependenciesToProcess.Num() > 0)
	{
		Task.NumDependencies.Add(DependenciesToProcess.Num());
		Task.Group.AddPendingTask(MoveTemp(TaskPtr));

		for (const FVoxelFutureValue* Dependency : DependenciesToProcess)
		{
			Dependency->State->GetStateImpl().AddDependentTask(Task);
		}

		return;
	}

	if (GVoxelBypassTaskQueue &&
		!bPrivateNeverBypassQueue)
	{
		const bool bCanExecute = INLINE_LAMBDA
		{
			switch (Task.Thread)
			{
			default: VOXEL_ASSUME(false);
			case EVoxelTaskThread::GameThread: return IsInGameThreadFast();
			case EVoxelTaskThread::RenderThread: return IsInRenderingThread();
			case EVoxelTaskThread::AsyncThread: return true;
			}
		};

		if (bCanExecute)
		{
			FVoxelTaskGroupScope Scope;
			if (!Scope.Initialize(Task.Group))
			{
				// Exiting
				return;
			}

			Task.Execute();
			return;
		}
	}

	Task.Group.ProcessTask(MoveTemp(TaskPtr));
}

FVoxelFutureValue FVoxelTaskFactory::Execute(const FVoxelPinType& Type, TVoxelUniqueFunction<FVoxelFutureValue()>&& Lambda)
{
	check(Type.IsValid());
	ensure(!Type.IsWildcard());

	const TSharedRef<FVoxelFutureValueStateImpl> State = MakeVoxelShared<FVoxelFutureValueStateImpl>(Type);

	Execute([State, Type, Name = PrivateName, Lambda = MoveTemp(Lambda)]
	{
		const FVoxelFutureValue Value = Lambda();

		if (!Value.IsValid())
		{
			State->SetValue(FVoxelRuntimePinValue(Type));
			return;
		}

		if (!Value.IsComplete())
		{
			if (!ensureVoxelSlow(Value.State->Type.CanBeCastedTo(Type)))
			{
				State->SetValue(FVoxelRuntimePinValue(Type));
				return;
			}

			Value.State->GetStateImpl().AddLinkedState(State);
			return;
		}

		FVoxelRuntimePinValue FinalValue = Value.GetValue_CheckCompleted();

		if (!ensureVoxelSlow(FinalValue.CanBeCastedTo(Type)))
		{
			FinalValue = FVoxelRuntimePinValue(Type);
		}

		if (!ensureMsgf(FinalValue.IsValidValue_Slow(), TEXT("Invalid value produced by %s"), *Name.ToString()))
		{
			FinalValue = FVoxelRuntimePinValue(Type);
		}

		State->SetValue(FinalValue);
	});

	return FVoxelFutureValue(State);
}