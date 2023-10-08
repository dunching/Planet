// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelQuery.h"
#include "VoxelRuntime.h"
#include "VoxelExecNode.h"
#include "VoxelSubsystem.h"
#include "VoxelDependency.h"
#include "VoxelQueryCache.h"
#include "VoxelExecNodeRuntimeWrapper.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelQueryContext);

const uint32 GVoxelQueryContextTLS = FPlatformTLS::AllocTlsSlot();

VOXEL_CONSOLE_VARIABLE(
	VOXELGRAPHCORE_API, int32, GVoxelMaxRecursionDepth, 8,
	"voxel.MaxRecursionDepth",
	"");

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimeInfoBase FVoxelRuntimeInfoBase::MakeFromWorld(UWorld* World)
{
	ensure(IsInGameThread());

	if (!ensure(World))
	{
		return {};
	}

	FVoxelRuntimeInfoBase Result;
	Result.WeakWorld = World;
	Result.bIsGameWorld = World->IsGameWorld();
	Result.bIsPreviewScene = World->WorldType == EWorldType::EditorPreview;
	Result.NetMode = World->GetNetMode();
	return Result;
}

FVoxelRuntimeInfoBase FVoxelRuntimeInfoBase::MakePreview()
{
	FVoxelRuntimeInfoBase Result;
	Result.bIsPreviewScene = true;
	return Result;
}

TSharedRef<FVoxelRuntimeInfo> FVoxelRuntimeInfoBase::MakeRuntimeInfo()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(WeakSubsystems.Num() == 0);

	ensure(!IsHiddenInEditorDependency);
	IsHiddenInEditorDependency = FVoxelDependency::Create(STATIC_FNAME("IsHiddenInEditor"), WeakInstance.IsValid() ? WeakInstance->GetFName() : FName());

	static const TArray<UScriptStruct*> SubsystemStructs = GetDerivedStructs<FVoxelSubsystem>();

	const TSharedRef<FVoxelRuntimeInfo> RuntimeInfoWithoutSubsystems = MakeVoxelShareable(new (GVoxelMemory) FVoxelRuntimeInfo(*this));
	for (UScriptStruct* Struct : SubsystemStructs)
	{
		const TSharedRef<FVoxelSubsystem> Subsystem = MakeSharedStruct<FVoxelSubsystem>(Struct);
		Subsystem->RuntimeInfo = RuntimeInfoWithoutSubsystems;
		if (!Subsystem->ShouldCreateSubsystem())
		{
			continue;
		}

		WeakSubsystems.Add(Struct, Subsystem);
		SubsystemRefs.Add(Subsystem);
	}
	RuntimeInfoWithoutSubsystems->Destroy();

	const TSharedRef<FVoxelRuntimeInfo> RuntimeInfo = MakeVoxelShareable(new (GVoxelMemory) FVoxelRuntimeInfo(*this));
	RuntimeInfo->Tick();

	for (const TSharedPtr<FVoxelSubsystem>& Subsystem : SubsystemRefs)
	{
		Subsystem->RuntimeInfo = RuntimeInfo;
		Subsystem->Create();
	}

	return RuntimeInfo;
}

bool FVoxelRuntimeInfoBase::IsHiddenInEditorImpl(const FVoxelQuery& Query) const
{
	Query.GetDependencyTracker().AddDependency(IsHiddenInEditorDependency.ToSharedRef());
	return bIsHiddenInEditor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelCallstack::ToDebugString() const
{
	TStringBuilderWithBuffer<TCHAR, NAME_SIZE> Result;

	TVoxelArray<const FVoxelCallstack*, TVoxelInlineAllocator<64>> Callstacks;
	for (const FVoxelCallstack* Callstack = this; Callstack; Callstack = Callstack->Parent.Get())
	{
		Callstacks.Add(Callstack);
	}

	for (int32 Index = Callstacks.Num() - 1; Index >= 0; Index--)
	{
		const FVoxelCallstack* Callstack = Callstacks[Index];
		if (Callstack->Node.IsExplicitlyNull())
		{
			continue;
		}

		if (Callstack->Node.EdGraphNodeTitle.IsNone())
		{
			ensure(!Callstack->Node.NodeId.IsNone());
			Callstack->Node.NodeId.AppendString(Result);
		}
		else
		{
			Callstack->Node.EdGraphNodeTitle.AppendString(Result);
		}

		if (Index > 0)
		{
			Result += " -> ";
		}
	}

	return FString(Result.ToView());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimeInfo::FVoxelRuntimeInfo(const FVoxelRuntimeInfoBase& RuntimeInfoBase)
	: FVoxelRuntimeInfoBase(RuntimeInfoBase)
{
}

FVoxelRuntimeInfo::~FVoxelRuntimeInfo()
{
	ensure(IsDestroyed());
	ensure(NumActiveTasks.GetValue() == 0);
}

void FVoxelRuntimeInfo::Destroy()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(!IsDestroyed());

	bDestroyStarted.Store(true);

	if (NumActiveTasks.GetValue() > 0)
	{
		VOXEL_SCOPE_COUNTER("Wait");

		double NextLogTime = FPlatformTime::Seconds() + 0.5;

		while (NumActiveTasks.GetValue() > 0)
		{
			if (NextLogTime < FPlatformTime::Seconds())
			{
				NextLogTime += 0.5;
				LOG_VOXEL(Log, "Destroy: Waiting for %d tasks", NumActiveTasks.GetValue());
			}

			FPlatformProcess::YieldThread();
		}
	}

	// Not always true, see FVoxelTaskGroupScope::Initialize
	// check(NumActiveTasks.GetValue() == 0);

	for (const TSharedPtr<FVoxelSubsystem>& Subsystem : SubsystemRefs)
	{
		Subsystem->Destroy();
	}

	// Make sure to empty, otherwise the subsystems RuntimeInfo will keep themselves alive
	SubsystemRefs.Empty();

	ensure(!IsDestroyed());
	bIsDestroyed.Store(true);
}

void FVoxelRuntimeInfo::Tick()
{
	VOXEL_FUNCTION_COUNTER();

#if WITH_EDITOR
	if (const AActor* Actor = Cast<AActor>(WeakInstance.Get()))
	{
		const bool bNewIsHiddenInEditor = Actor->IsHiddenEd();
		if (bIsHiddenInEditor != bNewIsHiddenInEditor)
		{
			bIsHiddenInEditor = bNewIsHiddenInEditor;
			IsHiddenInEditorDependency->Invalidate();
		}
	}
#endif

	for (const TSharedPtr<FVoxelSubsystem>& Subsystem : SubsystemRefs)
	{
		Subsystem->Tick();
	}
}

void FVoxelRuntimeInfo::AddReferencedObjects(FReferenceCollector& Collector)
{
	VOXEL_FUNCTION_COUNTER();

	for (const TSharedPtr<FVoxelSubsystem>& Subsystem : SubsystemRefs)
	{
		Subsystem->AddReferencedObjects(Collector);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelQueryContext> FVoxelQueryContext::Make(
	const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo)
{
	return MakeVoxelShareable(new (GVoxelMemory) FVoxelQueryContext(
		FVoxelNodePath(),
		MakeShared<FVoxelCallstack>(),
		RuntimeInfo,
		nullptr,
		FVoxelParameterPath(),
		nullptr,
		nullptr,
		nullptr,
		-1,
		nullptr));
}

TSharedRef<FVoxelQueryContext> FVoxelQueryContext::Make(
	const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
	const TSharedRef<const FVoxelParameterValues>& ParameterValues)
{
	return MakeVoxelShareable(new (GVoxelMemory) FVoxelQueryContext(
		FVoxelNodePath(),
		MakeShared<FVoxelCallstack>(),
		RuntimeInfo,
		nullptr,
		FVoxelParameterPath(),
		ParameterValues,
		nullptr,
		nullptr,
		-1,
		nullptr));
}

TSharedRef<FVoxelQueryContext> FVoxelQueryContext::EnterScope(const FVoxelGraphNodeRef& Node)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);

	TSharedPtr<FVoxelQueryContext>& Context = ScopeToChildQueryContext_RequiresLock.FindOrAdd(Node);
	if (!Context)
	{
		const TSharedRef<FVoxelCallstack> NewCallstack = MakeVoxelShared<FVoxelCallstack>();
		NewCallstack->Node = Node;
		NewCallstack->Parent = Callstack;

		Context = MakeVoxelShareable(new (GVoxelMemory) FVoxelQueryContext(
			Path,
			NewCallstack,
			RuntimeInfo,
			AsWeak(),
			ParameterPath,
			ParameterValues,
			ComputeInputContext,
			ComputeInputMap,
			GraphArrayIndex,
			GraphArray));
	}

	ensure(Context->Callstack->Node == Node);
	return Context.ToSharedRef();
}

TSharedRef<FVoxelQueryContext> FVoxelQueryContext::GetChildContext(const FVoxelChildQueryContextKey& Key)
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(GetDepth() < GVoxelMaxRecursionDepth);

	TSharedPtr<FVoxelQueryContext>& Context = KeyToChildQueryContext_RequiresLock.FindOrAdd(Key);
	if (!Context)
	{
		FVoxelNodePath NewPath = Path;
		NewPath.NodeRefs.Add(Key.Node);

		const TSharedRef<FVoxelCallstack> NewCallstack = MakeVoxelShared<FVoxelCallstack>();
		NewCallstack->Node = Key.Node;
		NewCallstack->Parent = Callstack;

		Context = MakeVoxelShareable(new (GVoxelMemory) FVoxelQueryContext(
			NewPath,
			NewCallstack,
			RuntimeInfo,
			AsWeak(),
			Key.ParameterPath,
			Key.ParameterValues,
			Key.ComputeInputContext,
			Key.ComputeInputMap,
			Key.GraphArrayIndex,
			Key.GraphArray));
	}

	ensure(Context->Path.NodeRefs.Last() == Key.Node);
	ensure(Context->Callstack->Node == Key.Node);
	ensure(Context->ParameterPath == Key.ParameterPath);
	ensure(Context->ParameterValues == Key.ParameterValues);
	ensure(Context->ComputeInputContext == Key.ComputeInputContext);
	ensure(Context->ComputeInputMap == Key.ComputeInputMap);
	ensure(Context->GraphArrayIndex == Key.GraphArrayIndex);
	ensure(Context->GraphArray == Key.GraphArray);

	return Context.ToSharedRef();
}

TSharedRef<FVoxelExecNodeRuntimeWrapper> FVoxelQueryContext::FindOrAddExecNodeRuntimeWrapper(const TSharedRef<FVoxelExecNode>& Node)
{
	TSharedPtr<FVoxelExecNodeRuntimeWrapper> NodeRuntimeWrapper;
	{
		VOXEL_SCOPE_LOCK(CriticalSection);

		TWeakPtr<FVoxelExecNodeRuntimeWrapper>& WeakNodeRuntimeWrapper = NodeIdToWeakRuntimeWrapper_RequiresLock.FindOrAdd(Node->GetNodeRef().NodeId);
		if (const TSharedPtr<FVoxelExecNodeRuntimeWrapper> PinnedNodeRuntimeWrapper = WeakNodeRuntimeWrapper.Pin())
		{
			return PinnedNodeRuntimeWrapper.ToSharedRef();
		}

		NodeRuntimeWrapper = MakeVoxelShared<FVoxelExecNodeRuntimeWrapper>(Node);
		WeakNodeRuntimeWrapper = NodeRuntimeWrapper;
	}
	NodeRuntimeWrapper->Initialize(AsShared());
	return NodeRuntimeWrapper.ToSharedRef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelQuery FVoxelQuery::Make(
	const TSharedRef<FVoxelQueryContext>& Context,
	const TSharedRef<const FVoxelQueryParameters>& Parameters,
	const TSharedRef<FVoxelDependencyTracker>& DependencyTracker)
{
	const TSharedRef<FSharedCache> SharedCache = MakeVoxelShared<FSharedCache>();

	return FVoxelQuery(
		Context->RuntimeInfo,
		Context,
		Parameters,
		DependencyTracker,
		Context->Callstack,
		SharedCache,
		SharedCache->GetQueryCache(*Context));
}

FVoxelQuery FVoxelQuery::EnterScope(const FVoxelGraphNodeRef& Node) const
{
	const TSharedRef<FVoxelCallstack> NewCallstack = MakeVoxelShared<FVoxelCallstack>();
	NewCallstack->Node = Node;
	NewCallstack->Parent = Callstack;

	return FVoxelQuery(
		QueryRuntimeInfo,
		Context,
		Parameters,
		DependencyTracker,
		NewCallstack,
		SharedCache,
		QueryCache);
}

FVoxelQuery FVoxelQuery::EnterScope(const FVoxelNode& Node) const
{
	return EnterScope(Node.GetNodeRef());
}

FVoxelQuery FVoxelQuery::MakeNewQuery(const TSharedRef<FVoxelQueryContext>& NewContext) const
{
	return FVoxelQuery(
		QueryRuntimeInfo,
		NewContext,
		Parameters,
		DependencyTracker,
		Callstack,
		SharedCache,
		SharedCache->GetQueryCache(*NewContext));
}

FVoxelQuery FVoxelQuery::MakeNewQuery(const TSharedRef<const FVoxelQueryParameters>& NewParameters) const
{
	// New parameters, invalidate cache
	const TSharedRef<FSharedCache> NewSharedCache = MakeVoxelShared<FSharedCache>();

	return FVoxelQuery(
		QueryRuntimeInfo,
		Context,
		NewParameters,
		DependencyTracker,
		Callstack,
		NewSharedCache,
		NewSharedCache->GetQueryCache(*Context));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelTransformRef FVoxelQuery::GetLocalToWorld() const
{
	return GetInfo(EVoxelQueryInfo::Local).GetLocalToWorld();
}

FVoxelTransformRef FVoxelQuery::GetQueryToWorld() const
{
	return GetInfo(EVoxelQueryInfo::Query).GetLocalToWorld();
}

FVoxelTransformRef FVoxelQuery::GetLocalToQuery() const
{
	return
		GetLocalToWorld() *
		GetQueryToWorld().Inverse();
}

FVoxelTransformRef FVoxelQuery::GetQueryToLocal() const
{
	return GetLocalToQuery().Inverse();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelQuery::FSharedCache::FSharedCache()
{
	RuntimeInfoAndNodePathToQueryCache_RequiresLock.Reserve(8);
}

FVoxelQueryCache& FVoxelQuery::FSharedCache::GetQueryCache(const FVoxelQueryContext& QueryContext) const
{
	VOXEL_SCOPE_LOCK(CriticalSection);

	TSharedPtr<FVoxelQueryCache>& Cache = RuntimeInfoAndNodePathToQueryCache_RequiresLock.FindOrAdd(
	{
		QueryContext.RuntimeInfo.ToWeakPtr(),
		QueryContext.Path
	});
	if (!Cache)
	{
		Cache = MakeVoxelShared<FVoxelQueryCache>();
	}
	return *Cache;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if ENABLE_VOXEL_QUERY_CHECKS
FVoxelFastCriticalSection GVoxelQueryCriticalSection;
TSet<FVoxelQuery*> GVoxelQueries;

VOXEL_RUN_ON_STARTUP_GAME(RegisterVoxelQueryChecks)
{
	GOnVoxelModuleUnloaded.AddLambda([]
	{
		checkf(GVoxelQueries.Num() == 0, TEXT("Leaked FVoxelQuery"));
	});
}
#endif

FVoxelQuery::FVoxelQuery(
	const TSharedRef<const FVoxelRuntimeInfo>& QueryRuntimeInfo,
	const TSharedRef<FVoxelQueryContext>& Context,
	const TSharedRef<const FVoxelQueryParameters>& Parameters,
	const TSharedRef<FVoxelDependencyTracker>& DependencyTracker,
	const TSharedRef<const FVoxelCallstack>& Callstack,
	const TSharedRef<const FSharedCache>& SharedCache,
	FVoxelQueryCache& QueryCache)
	: QueryRuntimeInfo(QueryRuntimeInfo)
	, Context(Context)
	, Parameters(Parameters)
	, DependencyTracker(DependencyTracker)
	, Callstack(Callstack)
	, SharedCache(SharedCache)
	, QueryCache(QueryCache)
{
#if ENABLE_VOXEL_QUERY_CHECKS
	VOXEL_SCOPE_LOCK(GVoxelQueryCriticalSection);
	GVoxelQueries.Add(this);
#endif
}

#if ENABLE_VOXEL_QUERY_CHECKS
FVoxelQuery::~FVoxelQuery()
{
	VOXEL_SCOPE_LOCK(GVoxelQueryCriticalSection);
	GVoxelQueries.Remove(this);
}

FVoxelQuery::FVoxelQuery(const FVoxelQuery& Other)
	: FVoxelQuery(
		Other.QueryRuntimeInfo,
		Other.Context,
		Other.Parameters,
		Other.DependencyTracker,
		Other.Callstack,
		Other.SharedCache,
		Other.QueryCache)
{
}

FVoxelQuery::FVoxelQuery(FVoxelQuery&& Other)
	: FVoxelQuery(
		Other.QueryRuntimeInfo,
		Other.Context,
		Other.Parameters,
		Other.DependencyTracker,
		Other.Callstack,
		Other.SharedCache,
		Other.QueryCache)
{
}
#endif