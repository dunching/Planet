// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelQuery.h"

class FVoxelNodeRuntime;
struct FVoxelRuntimeNode;
struct FVoxelGraphPinRef;
struct FVoxelGraphNodeRef;
struct IVoxelNodeInterface;

namespace Voxel::Graph
{
class FPin;
class FNode;
class FGraph;
enum class ENodeType : uint8;
}

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelNodeRuntime>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelNodeRuntime* NodeRuntime);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelNodeRuntime& NodeRuntime)
	{
		ProcessArg(Builder, &NodeRuntime);
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelNode>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelNode* Node);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelNode& Node)
	{
		ProcessArg(Builder, &Node);
	}
};

template<typename T>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<TSharedPtr<const T>, typename TEnableIf<TIsDerivedFrom<T, FVoxelNode>::Value>::Type>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TSharedPtr<const T>& Node)
	{
		FVoxelMessageArgProcessor::ProcessArg(Builder, Node.Get());
	}
};

template<typename T>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<TSharedRef<const T>, typename TEnableIf<TIsDerivedFrom<T, FVoxelNode>::Value>::Type>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const TSharedRef<const T>& Node)
	{
		FVoxelMessageArgProcessor::ProcessArg(Builder, Node.Get());
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelCallstack>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelCallstack& Callstack);
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelQuery>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelQuery& Query)
	{
		FVoxelMessageArgProcessor::ProcessArg(Builder, Query.GetCallstack());
	}
};

template<typename T>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<T, typename TEnableIf<TIsDerivedFrom<T, IVoxelNodeInterface>::Value && !std::is_same_v<T, FVoxelNode>>::Type>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T* Node)
	{
		if (!Node)
		{
			FVoxelMessageArgProcessor::ProcessArg(Builder, "<null>");
			return;
		}
		FVoxelMessageArgProcessor::ProcessArg(Builder, Node->GetNodeRef());
	}
	static void ProcessArg(FVoxelMessageBuilder& Builder, const T& Node)
	{
		FVoxelMessageArgProcessor::ProcessArg(Builder, Node.GetNodeRef());
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelGraphNodeRef>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelGraphNodeRef* NodeRef);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelGraphNodeRef& NodeRef)
	{
		ProcessArg(Builder, &NodeRef);
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelGraphPinRef>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelGraphPinRef* PinRef);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelGraphPinRef& PinRef)
	{
		ProcessArg(Builder, &PinRef);
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<FVoxelRuntimeNode>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelRuntimeNode* Node);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const FVoxelRuntimeNode& Node)
	{
		ProcessArg(Builder, &Node);
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<Voxel::Graph::FPin>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const Voxel::Graph::FPin* Pin);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const Voxel::Graph::FPin& Pin)
	{
		ProcessArg(Builder, &Pin);
	}
};

template<>
struct VOXELGRAPHCORE_API TVoxelMessageArgProcessor<Voxel::Graph::FNode>
{
	static void ProcessArg(FVoxelMessageBuilder& Builder, const Voxel::Graph::FNode* Node);
	static void ProcessArg(FVoxelMessageBuilder& Builder, const Voxel::Graph::FNode& Node)
	{
		ProcessArg(Builder, &Node);
	}
};