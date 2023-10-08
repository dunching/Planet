// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelRuntimeGraph.h"
#include "VoxelGraphExecutor.h"

class VOXELGRAPHCORE_API FVoxelGraphCompiler
{
public:
	using FPin = Voxel::Graph::FPin;
	using FNode = Voxel::Graph::FNode;
	using FGraph = Voxel::Graph::FGraph;
	using ENodeType = Voxel::Graph::ENodeType;
	using EPinDirection = Voxel::Graph::EPinDirection;

public:
	static TSharedPtr<FGraph> TranslateRuntimeGraph(const UVoxelRuntimeGraph& RuntimeGraph);

	static void RemoveSplitPins(FGraph& Graph, const UVoxelRuntimeGraph& RuntimeGraph);
	static void AddWildcardErrors(FGraph& Graph);
	static void AddNoDefaultErrors(FGraph& Graph);
	static void CheckParameters(const FGraph& Graph, const UVoxelGraph& VoxelGraph);
	static void CheckInputs(const FGraph& Graph, const UVoxelGraph& VoxelGraph);
	static void CheckOutputs(const FGraph& Graph, const UVoxelGraph& VoxelGraph);
	static void AddPreviewNode(FGraph& Graph, const UVoxelRuntimeGraph& RuntimeGraph);
	static void AddDebugNodes(FGraph& Graph, const UVoxelRuntimeGraph& RuntimeGraph);
	static void AddToBuffer(FGraph& Graph);
	static void RemoveLocalVariables(FGraph& Graph, const UVoxelGraph& VoxelGraph);
	static void CollapseInputs(FGraph& Graph);
	static void AddRootExecuteNode(FGraph& Graph, const UVoxelGraph& VoxelGraph);
	static void ReplaceTemplates(FGraph& Graph);
	static void RemovePassthroughs(FGraph& Graph);
	static void DisconnectVirtualPins(FGraph& Graph);
	static void RemoveUnusedNodes(FGraph& Graph);
	static void CheckForLoops(FGraph& Graph);

private:
	static bool ReplaceTemplatesImpl(FGraph& Graph);
	static void InitializeTemplatesPassthroughNodes(FGraph& Graph, FNode& Node);
};