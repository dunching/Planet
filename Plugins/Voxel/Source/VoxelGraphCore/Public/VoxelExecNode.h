// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExec.h"
#include "VoxelNode.h"
#include "VoxelExecNode.generated.h"

struct FVoxelSpawnable;
class FVoxelRuntime;
class FVoxelExecNodeRuntime;

USTRUCT(Category = "Exec Nodes", meta = (Abstract, NodeColor = "Red", NodeIconColor = "White", ShowInRootShortList))
struct VOXELGRAPHCORE_API FVoxelExecNode : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// If false, the node will never be executed
	VOXEL_INPUT_PIN(bool, EnableNode, true, VirtualPin);
	// If not connected, will be executed automatically
	VOXEL_OUTPUT_PIN(FVoxelExec, Exec, OptionalPin);

public:
	TSharedPtr<FVoxelExecNodeRuntime> CreateSharedExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const;

protected:
	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const VOXEL_PURE_VIRTUAL({});
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class IVoxelExecNodeRuntimeInterface
{
public:
	IVoxelExecNodeRuntimeInterface() = default;
	virtual ~IVoxelExecNodeRuntimeInterface() = default;

	virtual void Tick(FVoxelRuntime& Runtime) {}
	virtual void AddReferencedObjects(FReferenceCollector& Collector) {}
	virtual FVoxelOptionalBox GetBounds() const { return {}; }
};

class VOXELGRAPHCORE_API FVoxelExecNodeRuntime
	: public IVoxelExecNodeRuntimeInterface
	, public IVoxelNodeInterface
	, public FVoxelNodeAliases
	, public TSharedFromThis<FVoxelExecNodeRuntime>
	, public TVoxelRuntimeInfo<FVoxelExecNodeRuntime>
{
public:
	using NodeType = FVoxelExecNode;
	const TSharedRef<const FVoxelExecNode> NodeRef;

	explicit FVoxelExecNodeRuntime(const TSharedRef<const FVoxelExecNode>& NodeRef)
		: NodeRef(NodeRef)
	{
	}
	virtual ~FVoxelExecNodeRuntime() override;

	VOXEL_COUNT_INSTANCES();

	//~ Begin IVoxelNodeInterface Interface
	FORCEINLINE virtual const FVoxelGraphNodeRef& GetNodeRef() const final override
	{
		return NodeRef->GetNodeRef();
	}
	//~ End IVoxelNodeInterface Interface

	// True before Destroy is called
	bool IsDestroyed() const
	{
		return bIsDestroyed;
	}

	const FVoxelNodeRuntime& GetNodeRuntime() const
	{
		return NodeRef->GetNodeRuntime();
	}
	TSharedRef<FVoxelQueryContext> GetContext() const
	{
		return PrivateContext.ToSharedRef();
	}
	const TSharedRef<const FVoxelRuntimeInfo>& GetRuntimeInfo() const
	{
		return PrivateContext->RuntimeInfo;
	}
	FORCEINLINE const FVoxelRuntimeInfo& GetRuntimeInfoRef() const
	{
		checkVoxelSlow(PrivateContext);
		return *PrivateContext.Get()->RuntimeInfo;
	}

	TSharedPtr<FVoxelRuntime> GetRuntime() const;
	USceneComponent* GetRootComponent() const;

	virtual UScriptStruct* GetNodeType() const
	{
		return StaticStructFast<NodeType>();
	}

public:
	void CallCreate(
		const TSharedRef<FVoxelQueryContext>& Context,
		TVoxelMap<FName, FVoxelRuntimePinValue>&& ConstantValues);

	virtual void PreCreate() {}
	virtual void Create() {}
	virtual void Destroy() {}

protected:
	FORCEINLINE const FVoxelRuntimePinValue& GetConstantPin(const FVoxelPinRef& Pin) const
	{
		return PrivateConstantValues[Pin];
	}
	template<typename T>
	FORCEINLINE auto GetConstantPin(const TVoxelPinRef<T>& Pin) const -> decltype(auto)
	{
		if constexpr (VoxelPassByValue<T>)
		{
			return PrivateConstantValues[Pin].template Get<T>();
		}
		else
		{
			return PrivateConstantValues[Pin].template GetSharedStruct<T>();
		}
	}

private:
	bool bIsCreated = false;
	bool bIsDestroyed = false;
	TSharedPtr<FVoxelQueryContext> PrivateContext;
	TVoxelMap<FName, FVoxelRuntimePinValue> PrivateConstantValues;

	void CallDestroy();

	friend FVoxelExecNode;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename InNodeType>
class TVoxelExecNodeRuntime : public FVoxelExecNodeRuntime
{
public:
	using NodeType = InNodeType;
	using Super = TVoxelExecNodeRuntime;

	const NodeType& Node;

	explicit TVoxelExecNodeRuntime(const TSharedRef<const FVoxelExecNode>& NodeRef)
		: FVoxelExecNodeRuntime(NodeRef)
		, Node(CastChecked<NodeType>(*NodeRef))
	{
		checkStatic(TIsDerivedFrom<NodeType, FVoxelExecNode>::Value);
	}

	virtual UScriptStruct* GetNodeType() const override
	{
		return StaticStructFast<NodeType>();
	}
};