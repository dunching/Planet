// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphExecutor.h"
#include "VoxelGraph.h"
#include "VoxelGraphInstance.h"
#include "VoxelGraphCompiler.h"
#include "VoxelRootNode.h"
#include "VoxelDependency.h"
#include "VoxelParameterContainer.h"

FVoxelGraphExecutorManager* GVoxelGraphExecutorManager = MakeVoxelSingleton(FVoxelGraphExecutorManager);

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelGraphExecutor);
DEFINE_UNIQUE_VOXEL_ID(FVoxelGraphExecutorGlobalId);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFutureValue FVoxelGraphExecutor::Execute(const FVoxelQuery& Query) const
{
	if (!RootNode)
	{
		// Empty graph
		return {};
	}
	return RootNode->GetNodeRuntime().Get(RootNode->ValuePin, Query.EnterScope(*RootNode));
}

TSharedRef<FVoxelGraphExecutor> FVoxelGraphExecutor::MakeDummy()
{
	const TSharedRef<FVoxelGraphExecutorInfo> GraphExecutorInfo = MakeVoxelShared<FVoxelGraphExecutorInfo>();
#if WITH_EDITOR
	GraphExecutorInfo->Graph_EditorOnly = MakeVoxelShared<Voxel::Graph::FGraph>();
#endif

	return MakeVoxelShared<FVoxelGraphExecutor>(
		nullptr,
		GraphExecutorInfo,
		TVoxelAddOnlySet<TSharedPtr<const FVoxelNode>>());
}

TSharedPtr<FVoxelGraphExecutor> FVoxelGraphExecutor::Create(const FVoxelGraphPinRef& GraphPinRef)
{
	const UVoxelGraph* Graph = GraphPinRef.Node.GetGraph();
	if (!Graph)
	{
		// Don't return nullptr - this isn't a compilation error, just an empty graph
		return MakeDummy();
	}
	return Graph->GetRuntimeGraph().CreateExecutor(GraphPinRef);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphExecutorManager::Initialize()
{
	FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddLambda([]
	{
		GVoxelGraphExecutorManager->CleanupOldExecutors(10.f);
	});

	GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
	{
		GVoxelGraphExecutorManager->CleanupOldExecutors(-1.f);
	});
}

void FVoxelGraphExecutorManager::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	// Group all the invalidate calls
	const FVoxelDependencyInvalidationScope DependencyInvalidationScope;

	TSharedPtr<FExecutorRef> ExecutorRef;
	while (ExecutorRefsToUpdate.Dequeue(ExecutorRef))
	{
		if (!ExecutorRef->Executor_GameThread)
		{
			VOXEL_SCOPE_COUNTER_FORMAT("Recompiling %s", *ExecutorRef->Ref.ToString());
			ExecutorRef->SetExecutor_GameThread(FVoxelGraphExecutor::Create(ExecutorRef->Ref));
		}
	}

	CleanupOldExecutors(30.f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphExecutorManager::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelNullCheckReferenceCollector NullCheckCollector(Collector);

	for (const auto& It : MakeCopy(ExecutorRefs_GameThread))
	{
		const TSharedPtr<const FVoxelGraphExecutorInfo> ExecutorInfo = It.Value->ExecutorInfo_GameThread;
		if (!ExecutorInfo)
		{
			continue;
		}

		NullCheckCollector.AddReferencedObject(ConstCast(ExecutorInfo->RuntimeInfo));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphExecutorManager::RecompileAll()
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	const double StartTime = FPlatformTime::Seconds();
	ON_SCOPE_EXIT
	{
		LOG_VOXEL(Log, "RecompileAll took %fs", FPlatformTime::Seconds() - StartTime);
	};

	// Group all the invalidate calls
	const FVoxelDependencyInvalidationScope DependencyInvalidationScope;

	for (UVoxelRuntimeGraph* RuntimeGraph : TObjectRange<UVoxelRuntimeGraph>())
	{
		if (RuntimeGraph->IsTemplate())
		{
			continue;
		}

		RuntimeGraph->ForceRecompile();
	}

	for (const auto& It : MakeCopy(ExecutorRefs_GameThread))
	{
		FExecutorRef& ExecutorRef = *It.Value;

		if (It.Key.Node.IsDeleted())
		{
			ExecutorRefs_GameThread.Remove(It.Key);
			ExecutorRef.Dependency->Invalidate();
			continue;
		}

		ExecutorRef.SetExecutor_GameThread(FVoxelGraphExecutor::Create(It.Key));
	}
}

void FVoxelGraphExecutorManager::CleanupOldExecutors(const double TimeInSeconds)
{
	VOXEL_FUNCTION_COUNTER();

	const double Time = FPlatformTime::Seconds();
	for (auto It = ExecutorRefs_GameThread.CreateIterator(); It; ++It)
	{
		if (It.Value().IsUnique())
		{
			It.RemoveCurrent();
			continue;
		}

		if (It.Value()->LastUsedTime + TimeInSeconds < Time)
		{
			It.Value()->Executor_GameThread.Reset();
		}
	}
}

void FVoxelGraphExecutorManager::NotifyGraphChanged(const UVoxelGraphInterface& Graph)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	OnGraphChanged.Broadcast(Graph);

	RefreshExecutors([&](const FExecutorRef& ExecutorRef)
	{
		if (const TSharedPtr<const FVoxelGraphExecutorInfo> ExecutorInfo = ExecutorRef.ExecutorInfo_GameThread)
		{
			// Dummy won't have RuntimeInfo
			if (ExecutorInfo->RuntimeInfo &&
				ExecutorInfo->RuntimeInfo->ShouldRecompile(Graph))
			{
				return true;
			}
		}

		// Check the hierarchy for any potential match
		const UVoxelGraphInterface* ExecutorGraph = ExecutorRef.Ref.Node.Graph.Get();
		while (ExecutorGraph)
		{
			if (ExecutorGraph == &Graph)
			{
				return true;
			}

			const UVoxelGraphInstance* Instance = Cast<UVoxelGraphInstance>(ExecutorGraph);
			if (!Instance)
			{
				return false;
			}

			Instance->ParameterContainer->FixupProvider();
			ExecutorGraph = Instance->ParameterContainer->GetTypedProvider<UVoxelGraphInterface>();
		}

		return false;
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<const FVoxelComputeValue> FVoxelGraphExecutorManager::MakeCompute_GameThread(
	const FVoxelPinType Type,
	const FVoxelGraphPinRef& Ref,
	const bool bReturnExecutor)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());
	check(Type.IsValid());
	ensure(!Type.IsWildcard());

	const TSharedRef<FExecutorRef> ExecutorRef = MakeExecutorRef_GameThread(Ref);

	return MakeVoxelShared<FVoxelComputeValue>([=](const FVoxelQuery& Query) -> FVoxelFutureValue
	{
		ensureVoxelSlow(ExecutorRef.GetSharedReferenceCount() > 1);

		// Always add dependency, especially if we failed to compile
		Query.GetDependencyTracker().AddDependency(ExecutorRef->Dependency);

		const TVoxelFutureValue<FVoxelGraphExecutorRef> FutureExecutor = ExecutorRef->GetExecutor();

		if (bReturnExecutor)
		{
			return
				MakeVoxelTask()
				.Dependency(FutureExecutor)
				.Execute<FVoxelGraphExecutorRef>([=]
				{
					ensure(!ExecutorRef->bIsFirstExecutor_GameThread);

					const TSharedRef<FVoxelGraphExecutorRef> Result = MakeVoxelShared<FVoxelGraphExecutorRef>();
					Result->Executor = FutureExecutor.Get_CheckCompleted().Executor;
					return Result;
				});
		}

		// Always create a task to check Value's type
		return
			MakeVoxelTask()
			.Dependency(FutureExecutor)
			.Execute(Type, [=]() -> FVoxelFutureValue
			{
				ensure(!ExecutorRef->bIsFirstExecutor_GameThread);

				const TSharedPtr<const FVoxelGraphExecutor> Executor = FutureExecutor.Get_CheckCompleted().Executor;
				if (!Executor)
				{
					// Failed to compile
					return {};
				}

				FVoxelTaskReferencer::Get().AddExecutor(Executor.ToSharedRef());
				return Executor->Execute(Query);
			});
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelGraphExecutorManager::FExecutorRef::FExecutorRef(const FVoxelGraphPinRef& Ref)
	: Ref(Ref)
	, Dependency(FVoxelDependency::Create(STATIC_FNAME("GraphExecutor"), *Ref.ToString()))
{
}

TVoxelFutureValue<FVoxelGraphExecutorRef> FVoxelGraphExecutorManager::FExecutorRef::GetExecutor()
{
	LastUsedTime = FPlatformTime::Seconds();

	if (IsInGameThreadFast())
	{
		if (!Executor_GameThread.IsSet())
		{
			SetExecutor_GameThread(FVoxelGraphExecutor::Create(Ref));
		}

		const TSharedRef<FVoxelGraphExecutorRef> Struct = MakeVoxelShared<FVoxelGraphExecutorRef>();
		// Will be null if compilation failed
		Struct->Executor = Executor_GameThread.GetValue();
		return Struct;
	}

	VOXEL_SCOPE_LOCK(CriticalSection);

	if (const TSharedPtr<const FVoxelGraphExecutor> Executor = WeakExecutor_RequiresLock.Pin())
	{
		const TSharedRef<FVoxelGraphExecutorRef> Struct = MakeVoxelShared<FVoxelGraphExecutorRef>();
		Struct->Executor = Executor;
		return Struct;
	}

	if (!FutureValueState_RequiresLock.IsValid())
	{
		FutureValueState_RequiresLock = MakeVoxelShared<TVoxelFutureValueState<FVoxelGraphExecutorRef>>();
		GVoxelGraphExecutorManager->ExecutorRefsToUpdate.Enqueue(AsShared());
	}

	return TVoxelFutureValue<FVoxelGraphExecutorRef>(FVoxelFutureValue(FutureValueState_RequiresLock.ToSharedRef()));
}

void FVoxelGraphExecutorManager::FExecutorRef::SetExecutor_GameThread(const TSharedPtr<const FVoxelGraphExecutor>& NewExecutor)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInGameThread());

	GVoxelGraphExecutorManager->GlobalId = FVoxelGraphExecutorGlobalId::New();

	const TSharedPtr<const FVoxelGraphExecutorInfo> OldExecutorInfo = ExecutorInfo_GameThread;

	const bool bIsFirstExecutor = bIsFirstExecutor_GameThread;
	bIsFirstExecutor_GameThread = false;

	if (NewExecutor)
	{
		ExecutorInfo_GameThread = NewExecutor->Info;
		Executor_GameThread = NewExecutor;
	}
	else
	{
		ExecutorInfo_GameThread = nullptr;
		Executor_GameThread = nullptr;

		VOXEL_MESSAGE(Error, "{0} failed to compile", Ref);
	}

	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		WeakExecutor_RequiresLock = NewExecutor;

		if (FutureValueState_RequiresLock)
		{
			const TSharedRef<FVoxelGraphExecutorRef> Struct = MakeVoxelShared<FVoxelGraphExecutorRef>();
			Struct->Executor = NewExecutor;
			FutureValueState_RequiresLock->SetValue(Struct);
			FutureValueState_RequiresLock.Reset();
		}
	}

	// Outside of editor SetExecutor should never invalidate
	// Fully diff against old executor in case this SetExecutor is because it was GCed
#if WITH_EDITOR
	if (bIsFirstExecutor)
	{
		// Never invalidate if this is the first time we're setting the executor
		// Any call to GetExecutor will have made a future value waiting for this function to be called,
		// and thus nothing needs to be invalidated
		return;
	}

	if (ExecutorInfo_GameThread.IsValid() != OldExecutorInfo.IsValid())
	{
		Dependency->Invalidate();
		return;
	}

	if (!ExecutorInfo_GameThread.IsValid())
	{
		return;
	}

	if (!ensure(ExecutorInfo_GameThread->Graph_EditorOnly) ||
		!ensure(OldExecutorInfo->Graph_EditorOnly))
	{
		return;
	}

	if (ExecutorInfo_GameThread->Graph_EditorOnly->Identical(*OldExecutorInfo->Graph_EditorOnly))
	{
		return;
	}

	Dependency->Invalidate();
#endif
}

TSharedRef<FVoxelGraphExecutorManager::FExecutorRef> FVoxelGraphExecutorManager::MakeExecutorRef_GameThread(const FVoxelGraphPinRef& Ref)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	if (const TSharedPtr<FExecutorRef> ExecutorRef = ExecutorRefs_GameThread.FindRef(Ref))
	{
		return ExecutorRef.ToSharedRef();
	}

	const TSharedRef<FExecutorRef> ExecutorRef = MakeVoxelShared<FExecutorRef>(Ref);
	ExecutorRefs_GameThread.Add(Ref, ExecutorRef);

	if (GVoxelBypassTaskQueue)
	{
		ExecutorRef->SetExecutor_GameThread(FVoxelGraphExecutor::Create(ExecutorRef->Ref));
	}

	return ExecutorRef;
}

void FVoxelGraphExecutorManager::RefreshExecutors(const TFunctionRef<bool(const FExecutorRef& ExecutorRef)> ShouldRefresh)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	// Group all the invalidate calls
	const FVoxelDependencyInvalidationScope DependencyInvalidationScope;

	for (const auto& It : MakeCopy(ExecutorRefs_GameThread))
	{
		FExecutorRef& ExecutorRef = *It.Value;
		if (!ShouldRefresh(ExecutorRef))
		{
			continue;
		}

		if (It.Key.Node.IsDeleted())
		{
			LOG_VOXEL(Verbose, "%s deleted", *It.Key.ToString());

			ExecutorRefs_GameThread.Remove(It.Key);
			ExecutorRef.Dependency->Invalidate();
			continue;
		}

		LOG_VOXEL(Verbose, "Recompiling %s", *It.Key.ToString());

		const TSharedPtr<const FVoxelGraphExecutor> NewExecutor = FVoxelGraphExecutor::Create(It.Key);
		const TSharedPtr<const FVoxelGraphExecutorInfo> OldExecutorInfo = ExecutorRef.ExecutorInfo_GameThread;

		if (NewExecutor.IsValid() != OldExecutorInfo.IsValid())
		{
			LOG_VOXEL(Log, "Updating %s: compilation status changed", *It.Key.ToString());
			ExecutorRef.SetExecutor_GameThread(NewExecutor);
			continue;
		}

		if (!NewExecutor.IsValid())
		{
			// Failed to compile
			continue;
		}

#if WITH_EDITOR
		FString Diff;
		if (OldExecutorInfo->Graph_EditorOnly->Identical(*NewExecutor->Info->Graph_EditorOnly, &Diff))
		{
			// No changes
			continue;
		}
		LOG_VOXEL(Log, "Updating %s: %s", *It.Key.ToString(), *Diff);
#endif

		ExecutorRef.SetExecutor_GameThread(NewExecutor);
	}
}