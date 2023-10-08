// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelNode.h"

struct FVoxelNodeLibrary
{
public:
	FVoxelNodeLibrary();
	~FVoxelNodeLibrary();

	static const TVoxelArray<const FVoxelNode*>& GetNodes()
	{
		return Get().Nodes;
	}

	static const FVoxelNode* FindMakeNode(const FVoxelPinType& Type)
	{
		return Get().MakeNodes.FindRef(Type);
	}
	static const FVoxelNode* FindBreakNode(const FVoxelPinType& Type)
	{
		return Get().BreakNodes.FindRef(Type);
	}

	static const FVoxelNode* FindCastNode(const FVoxelPinType& From, const FVoxelPinType& To)
	{
		return Get().CastNodes.FindRef({ From, To });
	}

	template<typename T>
	static const FVoxelNode* GetNodeInstance()
	{
		return Get().StructToNodes.FindRef(T::StaticStruct());
	}

private:
	TVoxelArray<const FVoxelNode*> Nodes;
	TMap<FVoxelPinType, const FVoxelNode*> MakeNodes;
	TMap<FVoxelPinType, const FVoxelNode*> BreakNodes;
	TMap<TPair<FVoxelPinType, FVoxelPinType>, const FVoxelNode*> CastNodes;
	TMap<const UScriptStruct*, const FVoxelNode*> StructToNodes;

	static const FVoxelNodeLibrary& Get();
};