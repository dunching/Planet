// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelGraphNodeRef.generated.h"

class UVoxelGraph;
class UVoxelGraphInterface;
struct FVoxelGraphNodeRef;

struct VOXELGRAPHCORE_API FVoxelNodeNames
{
	static const FName Builtin;
	static const FName ExecuteNodeId;
	static const FName MergeNodeId;
	static const FName PreviewNodeId;
	static const FName GetPreviousOutputNodeId;
	static const FName MacroTemplateInput;
	static const FName MacroRecursiveTemplateInput;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelGraphNodeRef
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TWeakObjectPtr<const UVoxelGraphInterface> Graph;

	UPROPERTY()
	FName NodeId;

	UPROPERTY()
	int32 TemplateInstance = 0;

public:
	// Debug only
	UPROPERTY()
	FName EdGraphNodeTitle;

	// Debug only
	UPROPERTY()
	FName EdGraphNodeName;

	FVoxelGraphNodeRef() = default;
	FVoxelGraphNodeRef(
		const TWeakObjectPtr<const UVoxelGraphInterface>& Graph,
		const FName NodeId)
		: Graph(Graph)
		, NodeId(NodeId)
		, EdGraphNodeTitle(NodeId)
		, EdGraphNodeName(FVoxelNodeNames::Builtin)
	{
	}

	UVoxelGraph* GetGraph() const;
#if WITH_EDITOR
	UEdGraphNode* GetGraphNode_EditorOnly() const;
#endif

	bool IsDeleted() const;
	FVoxelGraphNodeRef WithSuffix(const FString& Suffix) const;

	FORCEINLINE bool IsExplicitlyNull() const
	{
		return
			Graph.IsExplicitlyNull() &&
			*this == FVoxelGraphNodeRef();
	}

	FORCEINLINE bool operator==(const FVoxelGraphNodeRef& Other) const
	{
		return
			Graph == Other.Graph &&
			NodeId == Other.NodeId &&
			TemplateInstance == Other.TemplateInstance;
	}
	FORCEINLINE bool operator!=(const FVoxelGraphNodeRef& Other) const
	{
		return
			Graph != Other.Graph ||
			NodeId != Other.NodeId ||
			TemplateInstance != Other.TemplateInstance;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FVoxelGraphNodeRef& Key)
	{
		Ar << Key.Graph;
		Ar << Key.NodeId;
		Ar << Key.TemplateInstance;
		Ar << Key.EdGraphNodeTitle;
		Ar << Key.EdGraphNodeName;
		return Ar;
	}
	FORCEINLINE friend uint32 GetTypeHash(const FVoxelGraphNodeRef& Key)
	{
		return FVoxelUtilities::MurmurHashMulti(
			GetTypeHash(Key.Graph),
			GetTypeHash(Key.NodeId),
			GetTypeHash(Key.TemplateInstance));
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelGraphPinRef
{
	GENERATED_BODY()

	UPROPERTY()
	FVoxelGraphNodeRef Node;

	UPROPERTY()
	FName PinName;

	FString ToString() const;

	FORCEINLINE bool operator==(const FVoxelGraphPinRef& Other) const
	{
		return
			Node == Other.Node &&
			PinName == Other.PinName;
	}
	FORCEINLINE bool operator!=(const FVoxelGraphPinRef& Other) const
	{
		return
			Node != Other.Node ||
			PinName != Other.PinName;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FVoxelGraphPinRef& Key)
	{
		return FVoxelUtilities::MurmurHashMulti(
			GetTypeHash(Key.Node),
			GetTypeHash(Key.PinName));
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelNodePath
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVoxelGraphNodeRef> NodeRefs;

	FORCEINLINE bool operator==(const FVoxelNodePath& Other) const
	{
		return NodeRefs == Other.NodeRefs;
	}
	FORCEINLINE bool operator!=(const FVoxelNodePath& Other) const
	{
		return NodeRefs != Other.NodeRefs;
	}

	FString ToDebugString() const;
	bool NetSerialize(FArchive& Ar, UPackageMap& Map);

	VOXELGRAPHCORE_API friend FArchive& operator<<(FArchive& Ar, FVoxelNodePath& Path);
	VOXELGRAPHCORE_API friend uint32 GetTypeHash(const FVoxelNodePath& Path);
};