// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelTaskGroup.h"
#include "VoxelTaskExecutor.h"
#include "VoxelNodeStats.h"
#include "EdGraph/EdGraphNode.h"

#if WITH_EDITOR
class FVoxelExecNodeStatManager
	: public FVoxelSingleton
	, public IVoxelNodeStatProvider
{
public:
	struct FQueuedStat
	{
		TSharedPtr<const FVoxelCallstack> Callstack;
		double Time = 0.;
	};
	TQueue<FQueuedStat, EQueueMode::Mpsc> Queue;

	TVoxelMap<TWeakObjectPtr<const UEdGraphNode>, double> NodeToTime;

	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override
	{
		GVoxelNodeStatProviders.Add(this);
	}
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();

		FQueuedStat QueuedStat;
		while (Queue.Dequeue(QueuedStat))
		{
			for (TSharedPtr<const FVoxelCallstack> Callstack = QueuedStat.Callstack; Callstack; Callstack = Callstack->Parent)
			{
				UEdGraphNode* GraphNode = Callstack->Node.GetGraphNode_EditorOnly();
				if (!GraphNode)
				{
					continue;
				}

				NodeToTime.FindOrAdd(GraphNode) += QueuedStat.Time;
			}
		}
	}
	//~ End FVoxelSingleton Interface

	//~ Begin IVoxelNodeStatProvider Interface
	virtual void ClearStats() override
	{
		NodeToTime.Empty();
	}
	virtual FText GetToolTip(const UEdGraphNode& Node) override
	{
		const double* Time = NodeToTime.Find(&Node);
		if (!Time)
		{
			return {};
		}

		return FText::Format(
			INVTEXT("Total execution time of this node & all its downstream nodes: {0}"),
			FVoxelUtilities::ConvertToTimeText(*Time, 9));
	}
	virtual FText GetText(const UEdGraphNode& Node) override
	{
		const double* Time = NodeToTime.Find(&Node);
		if (!Time)
		{
			return {};
		}

		return FText::Format(
			INVTEXT("Total Exec Time: {0}"),
			FVoxelUtilities::ConvertToTimeText(*Time));
	}
	//~ End IVoxelNodeStatProvider Interface
};
FVoxelExecNodeStatManager* GVoxelExecNodeStatManager = MakeVoxelSingleton(FVoxelExecNodeStatManager);
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelTaskGroup);

const uint32 GVoxelTaskGroupTLS = FPlatformTLS::AllocTlsSlot();

TSharedRef<FVoxelTaskGroup> FVoxelTaskGroup::Create(
	const FName Name,
	const FVoxelTaskPriority& Priority,
	const TSharedRef<FVoxelTaskReferencer>& Referencer,
	const TSharedRef<const FVoxelQueryContext>& Context)
{
	const TSharedRef<FVoxelTaskGroup> Group = MakeVoxelShareable(new (GVoxelMemory) FVoxelTaskGroup(
		Name,
		false,
		Priority,
		Referencer,
		Context));
	GVoxelTaskExecutor->AddGroup(Group);
	return Group;
}

TSharedRef<FVoxelTaskGroup> FVoxelTaskGroup::CreateSynchronous(
	const FName Name,
	const TSharedRef<FVoxelTaskReferencer>& Referencer,
	const TSharedRef<const FVoxelQueryContext>& Context)
{
	return MakeVoxelShareable(new (GVoxelMemory) FVoxelTaskGroup(
		Name,
		true,
		{},
		Referencer,
		Context));
}

TSharedRef<FVoxelTaskGroup> FVoxelTaskGroup::CreateSynchronous(
		const TSharedRef<FVoxelQueryContext>& Context)
{
	return CreateSynchronous(
		STATIC_FNAME("Synchronous"),
		MakeVoxelShared<FVoxelTaskReferencer>(STATIC_FNAME("Synchronous")),
		Context);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelTaskGroup::TryRunSynchronouslyGeneric(
	const TSharedRef<FVoxelQueryContext>& Context,
	const TFunctionRef<void()> Lambda,
	FString* OutError)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FVoxelTaskGroup> Group = CreateSynchronous(Context);

	FVoxelTaskGroupScope Scope;
	if (!ensure(Scope.Initialize(*Group)))
	{
		// Exiting
		if (OutError)
		{
			*OutError = "Runtime is being destroyed";
		}
		return false;
	}

	Lambda();

	return Group->TryRunSynchronously(OutError);
}

bool FVoxelTaskGroup::TryRunSynchronously(FString* OutError)
{
	VOXEL_FUNCTION_COUNTER();
	check(bIsSynchronous);

	const bool bRunGameTasks = IsInGameThread();
	const bool bRunRenderTasks = IsInRenderingThread();

	{
		TVoxelUniquePtr<FVoxelTask> Task;
		while (
			(bRunGameTasks && GameTasks.Dequeue(Task)) ||
			(bRunRenderTasks && RenderTasks.Dequeue(Task)) ||
			AsyncTasks.Dequeue(Task))
		{
			Task->Execute();
		}
	}

	bool bSuccess = true;
	TArray<FString> GameTaskNames;
	TArray<FString> RenderTaskNames;
	TArray<FString> PendingTaskNames;

	{
		TVoxelUniquePtr<FVoxelTask> Task;
		while (GameTasks.Dequeue(Task))
		{
			ensure(!bRunGameTasks);
			bSuccess = false;

			if (OutError)
			{
				GameTaskNames.Add(Task->Name.ToString());
			}
			else
			{
				break;
			}
		}
	}
	{
		TVoxelUniquePtr<FVoxelTask> Task;
		while (RenderTasks.Dequeue(Task))
		{
			ensure(!bRunRenderTasks);
			bSuccess = false;

			if (OutError)
			{
				RenderTaskNames.Add(Task->Name.ToString());
			}
			else
			{
				break;
			}
		}
	}
	{
		TVoxelUniquePtr<FVoxelTask> Task;
		while (AsyncTasks.Dequeue(Task))
		{
			ensure(false);
			bSuccess = false;
		}
	}

	{
		VOXEL_SCOPE_LOCK(PendingTasksCriticalSection);
		for (const TVoxelUniquePtr<FVoxelTask>& PendingTask : PendingTasks_RequiresLock)
		{
			bSuccess = false;

			if (OutError)
			{
				PendingTaskNames.Add(PendingTask->Name.ToString());
			}
			else
			{
				break;
			}
		}
	}

	if (bSuccess)
	{
		return true;
	}

	if (OutError)
	{
		*OutError = "Failed to process tasks synchronously. Tasks left:\n";

		if (GameTaskNames.Num() > 0)
		{
			*OutError += "Game tasks: " + FString::Join(GameTaskNames, TEXT(",")) + "\n";
		}
		if (RenderTaskNames.Num() > 0)
		{
			*OutError += "Render tasks: " + FString::Join(RenderTaskNames, TEXT(",")) + "\n";
		}
		if (PendingTaskNames.Num() > 0)
		{
			*OutError += "Pending tasks: " + FString::Join(PendingTaskNames, TEXT(",")) + "\n";
		}

		OutError->RemoveFromEnd("\n");
	}

	return false;
}

bool FVoxelTaskGroup::TryRunSynchronously_Ensure()
{
	ensure(IsInGameThread());
	FString Error;
	return ensureMsgf(TryRunSynchronously(&Error), TEXT("Failed to run %s synchronously: %s"), *Name.ToString(), *Error);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTaskGroup::StartAsyncTaskGeneric(
	const FName Name,
	const TSharedRef<FVoxelQueryContext>& Context,
	const FVoxelPinType& Type,
	TVoxelUniqueFunction<FVoxelFutureValue()> MakeFuture,
	TFunction<void(const FVoxelRuntimePinValue&)> Callback)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FVoxelTaskGroup> Group = Create(
		Name,
		FVoxelTaskPriority::MakeTop(),
		MakeVoxelShared<FVoxelTaskReferencer>(Name),
		Context);

	FVoxelTaskGroupScope Scope;
	if (!ensure(Scope.Initialize(*Group)))
	{
		return;
	}

	const FVoxelFutureValue Future =
		MakeVoxelTask(STATIC_FNAME("StartAsyncTask"))
		.Execute(Type, MoveTemp(MakeFuture));

	MakeVoxelTask()
	.Dependency(Future)
	.Execute([=]
	{
		// Keep group alive
		(void)Group;

		Callback(Future.GetValue_CheckCompleted());
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTaskGroup::ProcessTask(TVoxelUniquePtr<FVoxelTask> TaskPtr)
{
	checkVoxelSlow(TaskPtr->NumDependencies.GetValue() == 0);

	switch (TaskPtr->Thread)
	{
	default: VOXEL_ASSUME(false);
	case EVoxelTaskThread::GameThread:
	{
		if (IsInGameThreadFast())
		{
			FVoxelTaskGroupScope Scope;
			if (!Scope.Initialize(*this))
			{
				// Exiting
				return;
			}

			TaskPtr->Execute();
		}
		else
		{
			GameTasks.Enqueue(MoveTemp(TaskPtr));
		}
	}
	break;
	case EVoxelTaskThread::RenderThread:
	{
		// Enqueue to ensure commands enqueued before this will be run before it
		VOXEL_ENQUEUE_RENDER_COMMAND(FVoxelTaskProcessor_ProcessTask)(
			MakeWeakPtrLambda(this, [this, TaskPtrPtr = MakeUniqueCopy(MoveTemp(TaskPtr))](FRHICommandList& RHICmdList)
			{
				RenderTasks.Enqueue(MoveTemp(*TaskPtrPtr));
			}));
	}
	break;
	case EVoxelTaskThread::AsyncThread:
	{
		AsyncTasks.Enqueue(MoveTemp(TaskPtr));

		if (!AsyncProcessor.Load(std::memory_order_relaxed))
		{
			VOXEL_SCOPE_COUNTER("Trigger");
			GVoxelTaskExecutor->Event.Trigger();
		}
	}
	break;
	}
}

void FVoxelTaskGroup::AddPendingTask(TVoxelUniquePtr<FVoxelTask> TaskPtr)
{
	FVoxelTask& Task = *TaskPtr;
	checkVoxelSlow(Task.NumDependencies.GetValue() > 0);
	checkVoxelSlow(!Task.PendingTaskId.IsValid());

	VOXEL_SCOPE_LOCK(PendingTasksCriticalSection);

	Task.PendingTaskId = PendingTasks_RequiresLock.Add(MoveTemp(TaskPtr));
}

void FVoxelTaskGroup::OnDependencyComplete(FVoxelTask& Task)
{
	const int32 NumDependenciesLeft = Task.NumDependencies.Decrement();
	checkVoxelSlow(NumDependenciesLeft >= 0);

	if (NumDependenciesLeft > 0)
	{
		return;
	}

	TVoxelUniquePtr<FVoxelTask> TaskPtr;
	{
		VOXEL_SCOPE_LOCK(PendingTasksCriticalSection);

		if (!ensure(PendingTasks_RequiresLock.IsValidIndex(Task.PendingTaskId)))
		{
			return;
		}

		TaskPtr = MoveTemp(PendingTasks_RequiresLock[Task.PendingTaskId]);

		PendingTasks_RequiresLock.RemoveAt(Task.PendingTaskId);
		Task.PendingTaskId = {};
	}
	checkVoxelSlow(TaskPtr.Get() == &Task);

	ProcessTask(MoveTemp(TaskPtr));
}

void FVoxelTaskGroup::LogTasks() const
{
	VOXEL_FUNCTION_COUNTER();

	LOG_VOXEL(Log, "%s", *Name.ToString());

	if (!GameTasks.IsEmpty())
	{
		LOG_VOXEL(Log, "\tHas queued Game Tasks");
	}
	if (!RenderTasks.IsEmpty())
	{
		LOG_VOXEL(Log, "\tHas queued Render Tasks");
	}
	if (!AsyncTasks.IsEmpty())
	{
		LOG_VOXEL(Log, "\tHas queued Async Tasks");
	}

	VOXEL_SCOPE_LOCK(PendingTasksCriticalSection);

	for (const TVoxelUniquePtr<FVoxelTask>& PendingTask : PendingTasks_RequiresLock)
	{
		LOG_VOXEL(Log, "\tPending task %s: %d dependencies", *PendingTask->Name.ToString(), PendingTask->NumDependencies.GetValue());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTaskGroup::ProcessGameTasks()
{
	VOXEL_SCOPE_COUNTER_FNAME(Name);
	VOXEL_SCOPE_COUNTER_FNAME(InstanceStatName);
	VOXEL_SCOPE_COUNTER_FNAME(GraphStatName);
	VOXEL_SCOPE_COUNTER_FNAME(CallstackStatName);
	check(!bIsSynchronous);
	check(&Get() == this);
	const FVoxelQueryScope Scope(nullptr, &Context.Get());

	TVoxelUniquePtr<FVoxelTask> Task;
	while (
		!ShouldExit() &&
		GameTasks.Dequeue(Task))
	{
		Task->Execute();
	}
}

void FVoxelTaskGroup::ProcessRenderTasks(FRDGBuilder& GraphBuilder)
{
	VOXEL_SCOPE_COUNTER_FNAME(Name);
	VOXEL_SCOPE_COUNTER_FNAME(InstanceStatName);
	VOXEL_SCOPE_COUNTER_FNAME(GraphStatName);
	VOXEL_SCOPE_COUNTER_FNAME(CallstackStatName);
	check(!bIsSynchronous);
	check(&Get() == this);
	const FVoxelQueryScope Scope(nullptr, &Context.Get());

	TVoxelUniquePtr<FVoxelTask> Task;
	while (
		!ShouldExit() &&
		RenderTasks.Dequeue(Task))
	{
		RDG_EVENT_SCOPE(GraphBuilder, "%s", *Task->Name.ToString());

#if HAS_GPU_STATS
		static FDrawCallCategoryName DrawCallCategoryName;
		FRDGGPUStatScopeGuard StatScope(GraphBuilder, *Task->Name.ToString(), {}, nullptr, DrawCallCategoryName);
#endif

#if 0
		if (Task->Stat)
		{
			FVoxelShaderStatsScope::SetCallback([Stat = Task->Stat](int64 TimeInMicroSeconds, FName Name)
			{
				Stat->AddTime(
					Name == STATIC_FNAME("Readback")
					? FVoxelTaskStat::CopyGpuToCpu
					: FVoxelTaskStat::GpuCompute,
					TimeInMicroSeconds * 1000);
			});
		}
#endif

		Task->Execute();

#if 0
		if (Task->Stat)
		{
			FVoxelShaderStatsScope::SetCallback(nullptr);
		}
#endif
	}
}

void FVoxelTaskGroup::ProcessAsyncTasks()
{
	VOXEL_SCOPE_COUNTER_FNAME(Name);
	VOXEL_SCOPE_COUNTER_FNAME(InstanceStatName);
	VOXEL_SCOPE_COUNTER_FNAME(GraphStatName);
	VOXEL_SCOPE_COUNTER_FNAME(CallstackStatName);
	check(!bIsSynchronous);
	check(AsyncProcessor.Load());
	check(&Get() == this);
	const FVoxelQueryScope Scope(nullptr, &Context.Get());

	TVoxelUniquePtr<FVoxelTask> Task;
	while (
		!ShouldExit() &&
		AsyncTasks.Dequeue(Task))
	{
		Task->Execute();
	}
}

FVoxelTaskGroup::FVoxelTaskGroup(
	const FName Name,
	const bool bIsSynchronous,
	const FVoxelTaskPriority& Priority,
	const TSharedRef<FVoxelTaskReferencer>& Referencer,
	const TSharedRef<const FVoxelQueryContext>& Context)
	: Name(Name)
	, InstanceStatName(Context->RuntimeInfo->GetInstanceName())
	, GraphStatName(Context->RuntimeInfo->GetGraphPath())
	, CallstackStatName(Context->Callstack->ToDebugString())
	, bIsSynchronous(bIsSynchronous)
	, Priority(Priority)
	, Referencer(Referencer)
	, RuntimeInfo(Context->RuntimeInfo)
	, Context(Context)
{
	VOXEL_FUNCTION_COUNTER();
	PendingTasks_RequiresLock.Reserve(512);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTaskGroupScope::~FVoxelTaskGroupScope()
{
	if (!Group)
	{
		return;
	}

	ensure(Group->RuntimeInfo->NumActiveTasks.Decrement() >= 0);

	FPlatformTLS::SetTlsValue(GVoxelTaskGroupTLS, PreviousTLS);

#if WITH_EDITOR
	if (!GVoxelEnableNodeStats)
	{
		return;
	}

	const double Time = FPlatformTime::Seconds() - StartTime;
	GVoxelExecNodeStatManager->Queue.Enqueue(
	{
		Group->Context->Callstack,
		Time
	});
#endif
}

bool FVoxelTaskGroupScope::Initialize(FVoxelTaskGroup& NewGroup)
{
	if (NewGroup.RuntimeInfo->bDestroyStarted.Load())
	{
		return false;
	}

	NewGroup.RuntimeInfo->NumActiveTasks.Increment();

	if (NewGroup.RuntimeInfo->bDestroyStarted.Load())
	{
		ensure(Group->RuntimeInfo->NumActiveTasks.Decrement() >= 0);
		return false;
	}

	Group = NewGroup.AsShared();

	PreviousTLS = FPlatformTLS::GetTlsValue(GVoxelTaskGroupTLS);
	FPlatformTLS::SetTlsValue(GVoxelTaskGroupTLS, &NewGroup);

	return true;
}