// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinValue.h"
#include "VoxelCompiledGraph.h"
#include "VoxelRuntimeGraph.generated.h"

struct FVoxelNode;
class FVoxelGraphExecutor;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimePinRef
{
	GENERATED_BODY()

	UPROPERTY()
	FName NodeName;

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	bool bIsInput = false;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimePin
{
	GENERATED_BODY()

	UPROPERTY()
	FVoxelPinType Type;

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FName ParentPinName;

	UPROPERTY()
	FVoxelPinValue DefaultValue;

	UPROPERTY()
	TArray<FVoxelRuntimePinRef> LinkedTo;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimeNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
#if CPP
	TVoxelInstancedStruct<FVoxelNode> VoxelNode;
#else
	FVoxelInstancedStruct VoxelNode;
#endif

	UPROPERTY()
	TMap<FName, FVoxelRuntimePin> InputPins;

	UPROPERTY()
	TMap<FName, FVoxelRuntimePin> OutputPins;

public:
	UPROPERTY()
	FName StructName;

	UPROPERTY()
	FName EdGraphNodeName;

	UPROPERTY()
	FName EdGraphNodeTitle;

	UPROPERTY()
	TArray<FString> Errors;

public:
	FName GetNodeId() const;
	FVoxelRuntimePin* FindPin(const FName PinName, const bool bIsInput);
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimeGraphData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FName, FVoxelRuntimeNode> NodeNameToNode;

	UPROPERTY()
	TMap<FVoxelPinType, FVoxelInstancedStruct> TypeToMakeNode;

	UPROPERTY()
	TMap<FVoxelPinType, FVoxelInstancedStruct> TypeToBreakNode;

public:
	UPROPERTY()
	TSet<FName> DebuggedNodes;

	UPROPERTY()
	FVoxelRuntimePinRef PreviewedPin;

	UPROPERTY()
	FVoxelPinType PreviewedPinType;

	UPROPERTY()
	FVoxelInstancedStruct PreviewHandler;

public:
	const TMap<FName, FVoxelRuntimeNode>& GetNodeNameToNode() const
	{
		return NodeNameToNode;
	}

	FVoxelRuntimePin* FindPin(const FVoxelRuntimePinRef& Ref);
	const FVoxelRuntimePin* FindPin(const FVoxelRuntimePinRef& Ref) const
	{
		return ConstCast(this)->FindPin(Ref);
	}

	const FVoxelNode* FindMakeNode(const FVoxelPinType& Type) const;
	const FVoxelNode* FindBreakNode(const FVoxelPinType& Type) const;
};

#if WITH_EDITOR
class IVoxelGraphEditorCompiler
{
public:
	IVoxelGraphEditorCompiler() = default;
	virtual ~IVoxelGraphEditorCompiler() = default;

	virtual void CompileAll() = 0;
	virtual void ReconstructAllNodes(const UVoxelGraph& Graph) = 0;
	virtual FVoxelRuntimeGraphData Translate(const UVoxelGraph& Graph) = 0;
};

extern VOXELGRAPHCORE_API IVoxelGraphEditorCompiler* GVoxelGraphEditorCompiler;
#endif

UCLASS(Within=VoxelGraph)
class VOXELGRAPHCORE_API UVoxelRuntimeGraph : public UObject
{
	GENERATED_BODY()

public:
	const FVoxelRuntimeGraphData& GetData() const
	{
		return Data;
	}

	TSharedPtr<const Voxel::Graph::FGraph> GetRootGraph();
	TSharedPtr<FVoxelGraphExecutor> CreateExecutor(const FVoxelGraphPinRef& GraphPinRef);

	// Will clear compilation cache
	// Called whenever the graph is changed in the editor
	void ForceRecompile();
	bool ShouldRecompile(const UVoxelGraphInterface& GraphThatChanged) const;

public:
	template<typename T>
	bool HasNode() const
	{
		return HasNode(StaticStructFast<T>());
	}
	bool HasNode(const UScriptStruct* Struct) const;

public:
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

public:
	FSimpleMulticastDelegate OnMessagesChanged;

	void AddMessage(const TSharedRef<FTokenizedMessage>& Message);
	bool HasCompileMessages() const;

private:
	mutable TArray<TSharedRef<FTokenizedMessage>> CompileMessages;
	mutable TArray<TSharedRef<FTokenizedMessage>> RuntimeMessages;
	mutable TMap<FVoxelGraphPinRef, TArray<TSharedRef<FTokenizedMessage>>> PinRefToCompileMessages;

	friend class SVoxelGraphMessages;
	friend struct FVoxelGraphToolkit;

private:
	UPROPERTY(DuplicateTransient, NonTransactional)
	FVoxelRuntimeGraphData Data;

private:
	TOptional<TSharedPtr<const Voxel::Graph::FGraph>> CachedRootGraph;

	UPROPERTY(DuplicateTransient, NonTransactional, Transient)
	TSet<TObjectPtr<const UVoxelGraphInterface>> ReferencedGraphs;

	TSharedPtr<const Voxel::Graph::FGraph> CreateRootGraph() const;
};