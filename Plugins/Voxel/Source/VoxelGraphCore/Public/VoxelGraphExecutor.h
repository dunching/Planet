// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelGraphExecutor.generated.h"

struct FVoxelRootNode;
class UVoxelRuntimeGraph;
class FVoxelGraphExecutorManager;

struct VOXELGRAPHCORE_API FVoxelGraphExecutorInfo
{
#if WITH_EDITOR
	TSharedPtr<const Voxel::Graph::FGraph> Graph_EditorOnly;
#endif
	UVoxelRuntimeGraph* RuntimeInfo = nullptr;
};

class VOXELGRAPHCORE_API FVoxelGraphExecutor
{
public:
	const TSharedRef<const FVoxelGraphExecutorInfo> Info;
	const TVoxelAddOnlySet<TSharedPtr<const FVoxelNode>> Nodes;

	VOXEL_COUNT_INSTANCES();

	explicit FVoxelGraphExecutor(
		const TSharedPtr<const FVoxelRootNode>& RootNode,
		const TSharedRef<const FVoxelGraphExecutorInfo>& Info,
		TVoxelAddOnlySet<TSharedPtr<const FVoxelNode>>&& Nodes)
		: Info(Info)
		, Nodes(MoveTemp(Nodes))
		, RootNode(RootNode)
	{
	}

	FVoxelFutureValue Execute(const FVoxelQuery& Query) const;

public:
	static TSharedRef<FVoxelGraphExecutor> MakeDummy();
	static TSharedPtr<FVoxelGraphExecutor> Create(const FVoxelGraphPinRef& GraphPinRef);

private:
	const TSharedPtr<const FVoxelRootNode> RootNode;

};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelGraphExecutorRef
{
	GENERATED_BODY()

	TSharedPtr<const FVoxelGraphExecutor> Executor;
};

DECLARE_UNIQUE_VOXEL_ID(FVoxelGraphExecutorGlobalId);

extern VOXELGRAPHCORE_API FVoxelGraphExecutorManager* GVoxelGraphExecutorManager;

// Will keep pin default value objects alive
class VOXELGRAPHCORE_API FVoxelGraphExecutorManager : public FVoxelSingleton
{
public:
	FVoxelGraphExecutorGlobalId GlobalId;
	TMulticastDelegate<void(const UVoxelGraphInterface& Graph)> OnGraphChanged;

	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override;
	virtual void Tick() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FVoxelSingleton Interface

	void RecompileAll();
	void CleanupOldExecutors(double TimeInSeconds);
	void NotifyGraphChanged(const UVoxelGraphInterface& Graph);

	TSharedRef<const FVoxelComputeValue> MakeCompute_GameThread(
		const FVoxelPinType Type,
		const FVoxelGraphPinRef& Ref,
		bool bReturnExecutor = false);

private:
	class FExecutorRef : public TSharedFromThis<FExecutorRef>
	{
	public:
		const FVoxelGraphPinRef Ref;
		const TSharedRef<FVoxelDependency> Dependency;

		// Used for diffing & referenced objects, always alive
		TSharedPtr<const FVoxelGraphExecutorInfo> ExecutorInfo_GameThread;

		double LastUsedTime = FPlatformTime::Seconds();
		bool bIsFirstExecutor_GameThread = true;
		// Might hold references to other ExecutorRefs, will be cleared if unused after a few seconds
		// to avoid circular dependencies
		TOptional<TSharedPtr<const FVoxelGraphExecutor>> Executor_GameThread;

		explicit FExecutorRef(const FVoxelGraphPinRef& Ref);

		TVoxelFutureValue<FVoxelGraphExecutorRef> GetExecutor();
		void SetExecutor_GameThread(const TSharedPtr<const FVoxelGraphExecutor>& NewExecutor);

	private:
		FVoxelFastCriticalSection CriticalSection;
		TWeakPtr<const FVoxelGraphExecutor> WeakExecutor_RequiresLock;
		TSharedPtr<TVoxelFutureValueState<FVoxelGraphExecutorRef>> FutureValueState_RequiresLock;
	};
	TMap<FVoxelGraphPinRef, TSharedPtr<FExecutorRef>> ExecutorRefs_GameThread;
	TQueue<TSharedPtr<FExecutorRef>, EQueueMode::Mpsc> ExecutorRefsToUpdate;

	TSharedRef<FExecutorRef> MakeExecutorRef_GameThread(const FVoxelGraphPinRef& Ref);
	void RefreshExecutors(TFunctionRef<bool(const FExecutorRef& ExecutorRef)> ShouldRefresh);

	friend class FExecutorRef;
};