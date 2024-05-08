// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFutureValue.h"
#include "VoxelGraphNodeRef.h"
#include "VoxelTransformRef.h"
#include "VoxelParameterPath.h"
#include "VoxelQueryParameter.h"
#include "VoxelRuntimeParameter.h"

struct FVoxelNode;
struct FVoxelExecNode;
struct FVoxelSubsystem;
struct FVoxelTaskGroupScope;
class FVoxelRuntime;
class FVoxelDependency;
class FVoxelDependencyTracker;
class FVoxelQuery;
class FVoxelQueryCache;
class FVoxelQueryContext;
class FVoxelRuntimeInfo;
class FVoxelParameterValues;
class FVoxelInlineGraphData;
class FVoxelExecNodeRuntimeWrapper;

extern VOXELGRAPHCORE_API int32 GVoxelMaxRecursionDepth;

class VOXELGRAPHCORE_API FVoxelRuntimeInfoBase
{
public:
	TWeakObjectPtr<UWorld> WeakWorld;
	bool bIsGameWorld = false;
	bool bIsPreviewScene = false;
	bool bParallelTasks = false;
	ENetMode NetMode = {};

	TWeakPtr<FVoxelRuntime> WeakRuntime;
	TWeakObjectPtr<UObject> WeakInstance;
	TWeakObjectPtr<USceneComponent> WeakRootComponent;
	FString GraphPath;
	FString InstanceName;
	FVoxelRuntimeParameters Parameters;
	FVoxelTransformRef LocalToWorld;

	bool bIsHiddenInEditor = false;
	TSharedPtr<FVoxelDependency> IsHiddenInEditorDependency;

	TVoxelMap<UScriptStruct*, TWeakPtr<FVoxelSubsystem>> WeakSubsystems;
	TVoxelArray<TSharedPtr<FVoxelSubsystem>> SubsystemRefs;

	static FVoxelRuntimeInfoBase MakeFromWorld(UWorld* World);
	static FVoxelRuntimeInfoBase MakePreview();

	FVoxelRuntimeInfoBase& EnableParallelTasks()
	{
		bParallelTasks = true;
		return *this;
	}

	TSharedRef<FVoxelRuntimeInfo> MakeRuntimeInfo();

private:
	FVoxelRuntimeInfoBase() = default;

	bool IsHiddenInEditorImpl(const FVoxelQuery& Query) const;

	template<typename T>
	friend struct TVoxelRuntimeInfo;
};

template<typename T>
struct TVoxelRuntimeInfo
{
public:
	FORCEINLINE FObjectKey GetWorld() const
	{
		return  MakeObjectKey(GetRuntimeInfoBase().WeakWorld);
	}
	FORCEINLINE UWorld* GetWorld_GameThread() const
	{
		ensure(IsInGameThread());
		return GetRuntimeInfoBase().WeakWorld.Get();
	}
	FORCEINLINE bool IsGameWorld() const
	{
		return GetRuntimeInfoBase().bIsGameWorld;
	}
	FORCEINLINE bool IsPreviewScene() const
	{
		return GetRuntimeInfoBase().bIsPreviewScene;
	}
	FORCEINLINE bool ShouldRunTasksInParallel() const
	{
		return GetRuntimeInfoBase().bParallelTasks;
	}
	FORCEINLINE ENetMode GetNetMode() const
	{
		return GetRuntimeInfoBase().NetMode;
	}

public:
	// Will be null if we're in QueryVoxelChannel
	FORCEINLINE TSharedPtr<FVoxelRuntime> GetRuntime() const
	{
		return GetRuntimeInfoBase().WeakRuntime.Pin();
	}
	FORCEINLINE USceneComponent* GetRootComponent() const
	{
		ensure(IsInGameThread());
		return GetRuntimeInfoBase().WeakRootComponent.Get();
	}
	FORCEINLINE const FString& GetGraphPath() const
	{
		return GetRuntimeInfoBase().GraphPath;
	}
	FORCEINLINE const FString& GetInstanceName() const
	{
		return GetRuntimeInfoBase().InstanceName;
	}
	FORCEINLINE TWeakObjectPtr<UObject> GetInstance() const
	{
		return GetRuntimeInfoBase().WeakInstance;
	}
	FORCEINLINE const FVoxelRuntimeParameters& GetParameters() const
	{
		return GetRuntimeInfoBase().Parameters;
	}
	FORCEINLINE FVoxelTransformRef GetLocalToWorld() const
	{
		return GetRuntimeInfoBase().LocalToWorld;
	}
	FORCEINLINE FVoxelTransformRef GetWorldToLocal() const
	{
		return GetRuntimeInfoBase().LocalToWorld.Inverse();
	}
	FORCEINLINE bool IsHiddenInEditor(const FVoxelQuery& Query) const
	{
		return GetRuntimeInfoBase().IsHiddenInEditorImpl(Query);
	}
	FORCEINLINE const TVoxelMap<UScriptStruct*, TWeakPtr<FVoxelSubsystem>>& GetWeakSubsystems() const
	{
		return GetRuntimeInfoBase().WeakSubsystems;
	}

public:
	template<typename SubsystemType>
	FORCEINLINE TSharedPtr<SubsystemType> FindSubsystem() const
	{
		return StaticCastSharedPtr<SubsystemType>(GetWeakSubsystems().FindRef(StaticStructFast<SubsystemType>()).Pin());
	}
	template<typename RuntimeParameterType, typename = typename TEnableIf<TIsDerivedFrom<RuntimeParameterType, FVoxelRuntimeParameter>::Value>::Type>
	FORCEINLINE TSharedPtr<const RuntimeParameterType> FindParameter() const
	{
		return GetParameters().template Find<RuntimeParameterType>();
	}

private:
	FORCEINLINE const FVoxelRuntimeInfoBase& GetRuntimeInfoBase() const
	{
		return static_cast<const T&>(*this).GetRuntimeInfoRef();
	}
};

class VOXELGRAPHCORE_API FVoxelRuntimeInfo
	: private FVoxelRuntimeInfoBase
	, public TVoxelRuntimeInfo<FVoxelRuntimeInfo>
	, public TSharedFromThis<FVoxelRuntimeInfo>
{
public:
	~FVoxelRuntimeInfo();

	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		return *this;
	}
	FORCEINLINE bool ShouldExitTask() const
	{
		return bDestroyStarted.Load(std::memory_order_relaxed);
	}
	FORCEINLINE bool IsDestroyed() const
	{
		return bIsDestroyed.Load();
	}

	void Destroy();
	void Tick();
	void AddReferencedObjects(FReferenceCollector& Collector);

private:
	explicit FVoxelRuntimeInfo(const FVoxelRuntimeInfoBase& RuntimeInfoBase);

	TVoxelAtomic<bool> bDestroyStarted;
	TVoxelAtomic<bool> bIsDestroyed;

	FVoxelCacheLinePadding Padding;
	mutable FThreadSafeCounter NumActiveTasks;

	friend FVoxelTask;
	friend FVoxelTaskGroupScope;
	friend FVoxelRuntimeInfoBase;
	template<typename>
	friend struct TVoxelRuntimeInfo;
};

struct VOXELGRAPHCORE_API FVoxelCallstack
{
public:
	FVoxelGraphNodeRef Node;
	TSharedPtr<const FVoxelCallstack> Parent;

public:
	FORCEINLINE bool IsValid() const
	{
		return !Node.IsExplicitlyNull() || Parent;
	}

	FString ToDebugString() const;
};

using FVoxelComputePreviousOutput = TVoxelUniqueFunction<FVoxelFutureValue(const FVoxelQuery& Query, FName OutputName)>;

struct FVoxelComputeInput
{
	TSharedPtr<const FVoxelComputeValue> Compute;
	TSharedPtr<const FVoxelComputeValue> Compute_Executor;
};
using FVoxelComputeInputMap = TVoxelAddOnlyMap<FName, FVoxelComputeInput>;

struct FVoxelChildQueryContextKey
{
	FVoxelGraphNodeRef Node;
	FVoxelParameterPath ParameterPath;
	TSharedPtr<const FVoxelParameterValues> ParameterValues;
	TWeakPtr<FVoxelQueryContext> ComputeInputContext;
	TSharedPtr<const FVoxelComputeInputMap> ComputeInputMap;
	int32 GraphArrayIndex = -1;
	TSharedPtr<const TVoxelArray<TSharedRef<const FVoxelInlineGraphData>>> GraphArray;

	FVoxelChildQueryContextKey() = default;

	FVoxelChildQueryContextKey(
		const FVoxelGraphNodeRef& Node,
		const FVoxelParameterPath& ParameterPath,
		const TSharedPtr<const FVoxelParameterValues>& ParameterValues,
		const TWeakPtr<FVoxelQueryContext>& ComputeInputContext,
		const TSharedPtr<const FVoxelComputeInputMap>& ComputeInputMap,
		const int32 GraphArrayIndex,
		const TSharedPtr<const TVoxelArray<TSharedRef<const FVoxelInlineGraphData>>>& GraphArray)
		: Node(Node)
		, ParameterPath(ParameterPath)
		, ParameterValues(ParameterValues)
		, ComputeInputContext(ComputeInputContext)
		, ComputeInputMap(ComputeInputMap)
		, GraphArrayIndex(GraphArrayIndex)
		, GraphArray(GraphArray)
	{
	}

	FORCEINLINE bool operator==(const FVoxelChildQueryContextKey& Other) const
	{
		return
			Node == Other.Node &&
			ParameterPath == Other.ParameterPath &&
			ParameterValues == Other.ParameterValues &&
			ComputeInputContext == Other.ComputeInputContext &&
			ComputeInputMap == Other.ComputeInputMap &&
			GraphArrayIndex == Other.GraphArrayIndex &&
			GraphArray == Other.GraphArray;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelChildQueryContextKey& Key)
	{
		return
			GetTypeHash(Key.Node) ^
			GetTypeHash(Key.ParameterPath) ^
			GetTypeHash(Key.ParameterValues) ^
			GetTypeHash(Key.ComputeInputContext) ^
			GetTypeHash(Key.ComputeInputMap) ^
			GetTypeHash(Key.GraphArrayIndex) ^
			GetTypeHash(Key.GraphArray);
	}
};

// One context per graph
// Context chain up to the root runtime that created them
// When querying a channel, we might end up with different root contexts
class VOXELGRAPHCORE_API FVoxelQueryContext : public TSharedFromThis<FVoxelQueryContext>
{
public:
	// "vertical" callstack, ie FunctionCallA.FunctionCallB
	const FVoxelNodePath Path;
	// Full callstack, ie Execute -> FunctionCallA -> FunctionCallB -> MyExecNode
	const TSharedRef<const FVoxelCallstack> Callstack;
	// Info about the root runtime
	// Only case where this might be different within a single query is if we're querying a channel,
	// as each brush will have its own runtime
	const TSharedRef<const FVoxelRuntimeInfo> RuntimeInfo;
	// Parent context for debugging
	const TWeakPtr<const FVoxelQueryContext> ParentContext;

public:
	// Parameter path, used as prefix when in a macro with an inline graph
	const FVoxelParameterPath ParameterPath;
	// The parameter values of the graph/graph instance we're currently in
	const TSharedPtr<const FVoxelParameterValues> ParameterValues;

	// The context to use to query an Input pin
	// This is a parent context, but might not be the immediate parent in case we are a recursive macro template
	const TWeakPtr<FVoxelQueryContext> ComputeInputContext;
	// Map of input name to compute function
	const TSharedPtr<const FVoxelComputeInputMap> ComputeInputMap;

	// Index of the current graph in GraphArray, -1 if we're not in a recursive macro
	const int32 GraphArrayIndex;
	// If we're in a recursive macro, the graphs currently being processed
	const TSharedPtr<const TVoxelArray<TSharedRef<const FVoxelInlineGraphData>>> GraphArray;

public:
	static TSharedRef<FVoxelQueryContext> Make(
		const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo);

	static TSharedRef<FVoxelQueryContext> Make(
		const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
		const TSharedRef<const FVoxelParameterValues>& ParameterValues);

	VOXEL_COUNT_INSTANCES();

	FORCEINLINE int32 GetDepth() const
	{
		return Path.NodeRefs.Num();
	}

	TSharedRef<FVoxelQueryContext> EnterScope(const FVoxelGraphNodeRef& Node);
	TSharedRef<FVoxelQueryContext> GetChildContext(const FVoxelChildQueryContextKey& Key);
	TSharedRef<FVoxelExecNodeRuntimeWrapper> FindOrAddExecNodeRuntimeWrapper(const TSharedRef<FVoxelExecNode>& Node);

private:
	FVoxelQueryContext(
		const FVoxelNodePath& Path,
		const TSharedRef<const FVoxelCallstack>& Callstack,
		const TSharedRef<const FVoxelRuntimeInfo>& RuntimeInfo,
		const TWeakPtr<const FVoxelQueryContext>& ParentContext,
		const FVoxelParameterPath& ParameterPath,
		const TSharedPtr<const FVoxelParameterValues>& ParameterValues,
		const TWeakPtr<FVoxelQueryContext>& ComputeInputContext,
		const TSharedPtr<const FVoxelComputeInputMap>& ComputeInputMap,
		const int32 GraphArrayIndex,
		const TSharedPtr<const TVoxelArray<TSharedRef<const FVoxelInlineGraphData>>>& GraphArray)
		: Path(Path)
		, Callstack(Callstack)
		, RuntimeInfo(RuntimeInfo)
		, ParentContext(ParentContext)
		, ParameterPath(ParameterPath)
		, ParameterValues(ParameterValues)
		, ComputeInputContext(ComputeInputContext)
		, ComputeInputMap(ComputeInputMap)
		, GraphArrayIndex(GraphArrayIndex)
		, GraphArray(GraphArray)
	{
	}

	FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlyMap<FName, TWeakPtr<FVoxelExecNodeRuntimeWrapper>> NodeIdToWeakRuntimeWrapper_RequiresLock;
	// Make sure to keep all child contexts alive, even if they're just scope contexts
	TVoxelAddOnlyMap<FVoxelGraphNodeRef, TSharedPtr<FVoxelQueryContext>> ScopeToChildQueryContext_RequiresLock;
	TVoxelAddOnlyMap<FVoxelChildQueryContextKey, TSharedPtr<FVoxelQueryContext>> KeyToChildQueryContext_RequiresLock;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

enum class EVoxelQueryInfo : uint8
{
	Query,
	Local
};

#define ENABLE_VOXEL_QUERY_CHECKS VOXEL_DEBUG

class VOXELGRAPHCORE_API FVoxelQuery
{
public:
#if ENABLE_VOXEL_QUERY_CHECKS
	~FVoxelQuery();
	FVoxelQuery(const FVoxelQuery& Other);
	FVoxelQuery(FVoxelQuery&& Other);

	FVoxelQuery& operator=(const FVoxelQuery&) = delete;
	FVoxelQuery& operator=(FVoxelQuery&&) = delete;
#endif

	static FVoxelQuery Make(
		const TSharedRef<FVoxelQueryContext>& Context,
		const TSharedRef<const FVoxelQueryParameters>& Parameters,
		const TSharedRef<FVoxelDependencyTracker>& DependencyTracker);

	FVoxelQuery EnterScope(const FVoxelGraphNodeRef& Node) const;
	FVoxelQuery EnterScope(const FVoxelNode& Node) const;
	FVoxelQuery MakeNewQuery(const TSharedRef<FVoxelQueryContext>& NewContext) const;
	FVoxelQuery MakeNewQuery(const TSharedRef<const FVoxelQueryParameters>& NewParameters) const;

public:
	FORCEINLINE const FVoxelRuntimeInfo& GetInfo(const EVoxelQueryInfo Info) const
	{
		return Info == EVoxelQueryInfo::Query
			? *QueryRuntimeInfo
			: *Context->RuntimeInfo;
	}

	FORCEINLINE FVoxelQueryContext& GetContext() const
	{
		return Context.Get();
	}
	FORCEINLINE const FVoxelQueryParameters& GetParameters() const
	{
		return Parameters.Get();
	}
	FORCEINLINE FVoxelDependencyTracker& GetDependencyTracker() const
	{
		return DependencyTracker.Get();
	}

	FORCEINLINE const TSharedRef<FVoxelQueryContext>& GetSharedContext() const
	{
		return Context;
	}
	FORCEINLINE const TSharedRef<const FVoxelQueryParameters>& GetSharedParameters() const
	{
		return Parameters;
	}
	FORCEINLINE const TSharedRef<FVoxelDependencyTracker>& GetSharedDependencyTracker() const
	{
		return DependencyTracker;
	}

	FORCEINLINE const FVoxelCallstack& GetCallstack() const
	{
		return *Callstack;
	}
	FORCEINLINE FVoxelQueryCache& GetQueryCache() const
	{
		checkVoxelSlow(&QueryCache == &SharedCache->GetQueryCache(*Context));
		return QueryCache;
	}

public:
	FORCEINLINE FVoxelGraphNodeRef GetTopNode() const
	{
		return Callstack->Node;
	}
	FORCEINLINE TSharedRef<FVoxelQueryParameters> CloneParameters() const
	{
		return GetParameters().Clone();
	}

public:
	FVoxelTransformRef GetLocalToWorld() const;
	FVoxelTransformRef GetQueryToWorld() const;

	FVoxelTransformRef GetLocalToQuery() const;
	FVoxelTransformRef GetQueryToLocal() const;

private:
	struct FSharedCache
	{
		mutable FVoxelFastCriticalSection CriticalSection;
		// Key by runtime and path
		// Path might be empty or the same for different runtimes when working with brushes
		mutable TVoxelAddOnlyMap<TPair<TWeakPtr<const FVoxelRuntimeInfo>, FVoxelNodePath>, TSharedPtr<FVoxelQueryCache>> RuntimeInfoAndNodePathToQueryCache_RequiresLock;

		FSharedCache();
		FVoxelQueryCache& GetQueryCache(const FVoxelQueryContext& QueryContext) const;
	};

	// The RuntimeInfo of the object making this query
	// Typically if we're generating a voxel mesh this would be the voxel actor currently generating the mesh
	const TSharedRef<const FVoxelRuntimeInfo> QueryRuntimeInfo;
	// One context per graph
	// Context chain up to the root runtime that created them
	// When querying a channel, we might end up with different root contexts
	const TSharedRef<FVoxelQueryContext> Context;
	// The transient query parameters such as position, LOD etc
	const TSharedRef<const FVoxelQueryParameters> Parameters;
	const TSharedRef<FVoxelDependencyTracker> DependencyTracker;
	const TSharedRef<const FVoxelCallstack> Callstack;
	const TSharedRef<const FSharedCache> SharedCache;
	FVoxelQueryCache& QueryCache;

	FVoxelQuery(
		const TSharedRef<const FVoxelRuntimeInfo>& QueryRuntimeInfo,
		const TSharedRef<FVoxelQueryContext>& Context,
		const TSharedRef<const FVoxelQueryParameters>& Parameters,
		const TSharedRef<FVoxelDependencyTracker>& DependencyTracker,
		const TSharedRef<const FVoxelCallstack>& Callstack,
		const TSharedRef<const FSharedCache>& SharedCache,
		FVoxelQueryCache& QueryCache);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern VOXELGRAPHCORE_API const uint32 GVoxelQueryContextTLS;

class VOXELGRAPHCORE_API FVoxelQueryScope
{
public:
	const FVoxelQueryScope* const Parent;
	const FVoxelQuery* const Query;
	const FVoxelQueryContext* const QueryContext;

	FORCEINLINE FVoxelQueryScope(
		const FVoxelQuery* Query,
		const FVoxelQueryContext* QueryContext)
		: Parent(TryGet())
		, Query(Query)
		, QueryContext(QueryContext)
	{
		FPlatformTLS::SetTlsValue(GVoxelQueryContextTLS, this);
	}
	FORCEINLINE explicit FVoxelQueryScope(const FVoxelQuery& Query)
		: FVoxelQueryScope(&Query, &Query.GetContext())
	{
	}
	FORCEINLINE ~FVoxelQueryScope()
	{
		FPlatformTLS::SetTlsValue(GVoxelQueryContextTLS, const_cast<FVoxelQueryScope*>(Parent));
	}

	FORCEINLINE static const FVoxelQueryScope* TryGet()
	{
		return static_cast<const FVoxelQueryScope*>(FPlatformTLS::GetTlsValue(GVoxelQueryContextTLS));
	}
};