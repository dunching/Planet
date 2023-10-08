// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelTaskExecutor.h"
#include "VoxelMemoryScope.h"
#include "Engine/Engine.h"

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, int32, GVoxelNumThreads, 2,
	"voxel.NumThreads",
	"The number of threads to use to process voxel tasks");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelHideTaskCount, false,
	"voxel.HideTaskCount",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelLogTaskTimes, false,
	"voxel.LogTaskTimes",
	"If true, will log how long it's been since the task queue was empty");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, bool, GVoxelBenchmark, false,
	"voxel.Benchmark",
	"If true, will continuously refresh the world as soon as it's done processing");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, int32, GVoxelThreadingThreadPriority, 2,
	"voxel.threading.ThreadPriority",
	"0: Normal"
	"1: AboveNormal"
	"2: BelowNormal"
	"3: Highest"
	"4: Lowest"
	"5: SlightlyBelowNormal"
	"6: TimeCritical");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, float, GVoxelThreadingPriorityDuration, 0.5f,
	"voxel.threading.PriorityDuration",
	"Task priorities will be recomputed with the new camera position every PriorityDuration seconds");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, int32, GVoxelThreadingMaxConcurrentRenderTasks, 512,
	"voxel.threading.MaxConcurrentRenderTasks",
	"");

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, int32, GVoxelThreadingMaxSortedTasks, 4096,
	"voxel.threading.MaxSortedTasks",
	"");

VOXEL_CONSOLE_COMMAND(
	LogAllTasks,
	"voxel.LogAllTasks",
	"")
{
	GVoxelTaskExecutor->LogAllTasks();
}

FVoxelTaskExecutor* GVoxelTaskExecutor = MakeVoxelSingleton(FVoxelTaskExecutor);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTaskExecutor::LogAllTasks()
{
	Groups.ForeachGroup([&](const FVoxelTaskGroup& Group)
	{
		Group.LogTasks();
		return true;
	});
}

void FVoxelTaskExecutor::AddGroup(const TSharedRef<FVoxelTaskGroup>& Group)
{
	if (IsExiting())
	{
		return;
	}

	if (Groups.Num() == 0)
	{
		FVoxelUtilities::RunOnGameThread([this]
		{
			OnBeginProcessing.Broadcast();
		});
	}

	Groups.Add(Group);
	Event.Trigger();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelTaskExecutor::Initialize()
{
	TFunction<void()> Callback = [this]
	{
		bIsExiting.Store(true);
		Groups.Reset();

		VOXEL_SCOPE_LOCK(ThreadsCriticalSection);
		Threads.Reset();
	};

	FCoreDelegates::OnPreExit.AddLambda(Callback);
	FCoreDelegates::OnExit.AddLambda(Callback);
	GOnVoxelModuleUnloaded_DoCleanup.AddLambda(Callback);

	FVoxelRenderUtilities::OnPreRender().AddLambda([this](FRDGBuilder& GraphBuilder)
	{
		Tick_RenderThread(GraphBuilder);
	});
}

void FVoxelTaskExecutor::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	if (IsExiting())
	{
		return;
	}

	const int32 CurrentNumTasks = NumTasks();

	if (!GVoxelHideTaskCount &&
		CurrentNumTasks > 0)
	{
		const FString Message = FString::Printf(TEXT("%d voxel tasks left using %d threads"), CurrentNumTasks, GVoxelNumThreads);
		GEngine->AddOnScreenDebugMessage(uint64(0x557D0C945D26), FApp::GetDeltaTime() * 1.5f, FColor::White, Message);
	}

	if (GVoxelLogTaskTimes &&
		bWasProcessingTaskLastFrame &&
		CurrentNumTasks == 0)
	{
		LOG_VOXEL(Log, "Tasks took %.3fs", FPlatformTime::Seconds() - LastNoTasksTime);
	}

	if (GVoxelBenchmark &&
		bWasProcessingTaskLastFrame &&
		CurrentNumTasks == 0)
	{
		const double Time = FPlatformTime::Seconds() - LastNoTasksTime;
		LOG_VOXEL(Log, "Tasks took %.3fs", Time);
		GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Green, FString::Printf(TEXT("Tasks took %.3fs"), Time));

		GEngine->Exec(nullptr, TEXT("voxel.RefreshAll"));
	}

	if (CurrentNumTasks == 0)
	{
		LastNoTasksTime = FPlatformTime::Seconds();

		if (bWasProcessingTaskLastFrame)
		{
			OnEndProcessing.Broadcast();
		}
	}
	bWasProcessingTaskLastFrame = CurrentNumTasks > 0;

	GVoxelNumThreads = FMath::Max(GVoxelNumThreads, 1);

	if (Threads.Num() != GVoxelNumThreads)
	{
		AsyncVoxelTask([this]
		{
			VOXEL_SCOPE_LOCK(ThreadsCriticalSection);

			while (Threads.Num() < GVoxelNumThreads)
			{
				Threads.Add(MakeUnique<FThread>());
				Event.Trigger();
			}

			while (Threads.Num() > GVoxelNumThreads)
			{
				Threads.Pop(false);
			}
		});
	}

	{
		VOXEL_SCOPE_COUNTER("Process game groups");

		// Max 100ms tick
		const double EndTime = FPlatformTime::Seconds() + 0.1;

		TWeakPtr<FVoxelTaskGroup> WeakGroup;
		while (
			FPlatformTime::Seconds() < EndTime &&
			GameGroupsQueue.Dequeue(WeakGroup))
		{
			TSharedPtr<FVoxelTaskGroup> TmpGroup = WeakGroup.Pin();
			if (!TmpGroup)
			{
				continue;
			}

			FVoxelTaskGroupScope Scope;
			if (!Scope.Initialize(*TmpGroup))
			{
				// Exiting
				continue;
			}

			// Reset to only have one valid group ref for ShouldExit
			TmpGroup.Reset();

			Scope.GetGroup().ProcessGameTasks();
		}
	}

	AsyncVoxelTask([this]
	{
		VOXEL_SCOPE_COUNTER("Gather game tasks");
		Groups.ForeachGroup([&](FVoxelTaskGroup& Group)
		{
			if (Group.HasGameTasks())
			{
				GameGroupsQueue.Enqueue(Group.AsWeak());
			}
			return true;
		});
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_GPU_STAT(FVoxelTaskExecutor);

void FVoxelTaskExecutor::Tick_RenderThread(FRDGBuilder& GraphBuilder)
{
	if (IsExiting())
	{
		return;
	}

	VOXEL_FUNCTION_COUNTER();
	RDG_GPU_STAT_SCOPE(GraphBuilder, FVoxelTaskExecutor);

	for (int32 Index = 0; Index < GVoxelThreadingMaxConcurrentRenderTasks; Index++)
	{
		TSharedPtr<FVoxelTaskGroup> GroupToProcess;
		Groups.ForeachGroup([&](FVoxelTaskGroup& Group)
		{
			if (!Group.HasRenderTasks())
			{
				return true;
			}

			ensure(!GroupToProcess);
			GroupToProcess = Group.AsShared();
			return false;
		});

		if (!GroupToProcess)
		{
			return;
		}

		FVoxelTaskGroupScope Scope;
		if (!Scope.Initialize(*GroupToProcess))
		{
			// Exiting
			return;
		}

		// Reset to only have one valid group ref for ShouldExit
		GroupToProcess.Reset();

		Scope.GetGroup().ProcessRenderTasks(GraphBuilder);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTaskExecutor::FTaskGroupArray::FTaskGroupArray()
{
	Groups.Reserve(2048);
}

void FVoxelTaskExecutor::FTaskGroupArray::Add(const TSharedRef<FVoxelTaskGroup>& NewGroup)
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelScopeLock_Write Lock(CriticalSection);

	if (GVoxelTaskExecutor->IsExiting())
	{
		return;
	}

#if VOXEL_DEBUG && 0
	for (const FTaskGroup& Group : Groups)
	{
		ensure(Group.Data->WeakGroup != NewGroup);
	}
#endif

	FTaskGroup TaskGroup(NewGroup);

	if (Groups.Num() > GVoxelThreadingMaxSortedTasks)
	{
		bSorted = false;
	}

	if (bSorted)
	{
		const int32 InsertLocation = VOXEL_INLINE_COUNTER("LowerBound", Algo::LowerBound(Groups, TaskGroup, TLess<>()));

		VOXEL_SCOPE_COUNTER("Insert");
		Groups.Insert(MoveTemp(TaskGroup), InsertLocation);
		//checkVoxelSlow(Algo::IsSorted(Groups));
	}
	else
	{
		Groups.Add(MoveTemp(TaskGroup));
	}

	ValidGroups.Reset();
	ValidGroups.SetNum(Groups.Num(), true);

	NumGroups.Set(Groups.Num());
}

void FVoxelTaskExecutor::FTaskGroupArray::Reset()
{
	FVoxelScopeLock_Write Lock(CriticalSection);
	bSorted = true;
	LastPriorityComputeTime = 0;
	Groups.Reset();
	ValidGroups.Reset();
}

void FVoxelTaskExecutor::FTaskGroupArray::UpdatePriorities()
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelScopeLock_Write Lock(CriticalSection);

	LastPriorityComputeTime = FPlatformTime::Seconds();

	Groups.RemoveAllSwap([](const FTaskGroup& TaskGroup)
	{
		return !TaskGroup.Data->WeakGroup.IsValid();
	}, false);

	ValidGroups.Reset();
	ValidGroups.SetNum(Groups.Num(), true);

	NumGroups.Set(Groups.Num());

	if (Groups.Num() > GVoxelThreadingMaxSortedTasks)
	{
		bSorted = false;
		return;
	}

	bSorted = true;

	ParallelFor(Groups, [&](FTaskGroup& TaskGroup)
	{
		TaskGroup.PriorityValue = TaskGroup.Data->Priority.GetPriority();
	});

	VOXEL_SCOPE_COUNTER("Sort");
	Groups.Sort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTaskExecutor::FThread::FThread()
{
	UE::Trace::ThreadGroupBegin(TEXT("VoxelThreadPool"));

	static int32 ThreadIndex = 0;
	const FString Name = FString::Printf(TEXT("Voxel Thread %d"), ThreadIndex++);

	Thread = FRunnableThread::Create(
		this,
		*Name,
		1024 * 1024 * (GVoxelBypassTaskQueue ? 128 : 1),
		EThreadPriority(FMath::Clamp(GVoxelThreadingThreadPriority, 0, 6)),
		FPlatformAffinity::GetPoolThreadMask());

	UE::Trace::ThreadGroupEnd();
}

FVoxelTaskExecutor::FThread::~FThread()
{
	VOXEL_FUNCTION_COUNTER();

	// Tell the thread it needs to die
	bTimeToDie.Store(true);
	// Trigger the thread so that it will come out of the wait state if
	// it isn't actively doing work
	GVoxelTaskExecutor->Event.Trigger();
	// Kill (but wait for thread to finish)
	Thread->Kill(true);
	// Delete
	delete Thread;
}

uint32 FVoxelTaskExecutor::FThread::Run()
{
	VOXEL_LLM_SCOPE();

	const TUniquePtr<FVoxelMemoryScope> MemoryScope = MakeUnique<FVoxelMemoryScope>();

Wait:
	if (bTimeToDie.Load())
	{
		return 0;
	}

	if (!VOXEL_ALLOW_MALLOC_INLINE(GVoxelTaskExecutor->Event.Wait(10)))
	{
		MemoryScope->Clear();
		goto Wait;
	}

GetNextTask:
	if (bTimeToDie.Load())
	{
		return 0;
	}

	TSharedPtr<FVoxelTaskGroup> TmpGroup = GVoxelTaskExecutor->GetGroupToProcess(this);
	if (!TmpGroup)
	{
		goto Wait;
	}

	FVoxelTaskGroupScope Scope;
	if (!Scope.Initialize(*TmpGroup))
	{
		// Exiting
		goto Wait;
	}

	// Reset to only have one valid group ref for ShouldExit
	TmpGroup.Reset();

	checkVoxelSlow(Scope.GetGroup().AsyncProcessor.Load() == this);

	Scope.GetGroup().ProcessAsyncTasks();

	const void* Old = Scope.GetGroup().AsyncProcessor.Exchange(nullptr);
	checkVoxelSlow(Old == this);

	goto GetNextTask;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<FVoxelTaskGroup> FVoxelTaskExecutor::GetGroupToProcess(const FThread* Thread)
{
	VOXEL_FUNCTION_COUNTER();

	TSharedPtr<FVoxelTaskGroup> GroupToProcess;
	Groups.ForeachGroup([&](FVoxelTaskGroup& Group)
	{
		if (!Group.HasAsyncTasks())
		{
			return true;
		}

		const void* Expected = nullptr;
		if (!Group.AsyncProcessor.CompareExchangeStrong(Expected, Thread))
		{
			return true;
		}
		checkVoxelSlow(Expected == nullptr);

		ensure(!GroupToProcess);
		GroupToProcess = Group.AsShared();
		return false;
	});

	return GroupToProcess;
}