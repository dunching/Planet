// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelRuntimeGraph.h"

class FVoxelGraphEditorCompiler : public IVoxelGraphEditorCompiler
{
public:
	//~ Begin IVoxelGraphEditorCompiler Interface
	virtual void CompileAll() override;
	virtual void ReconstructAllNodes(const UVoxelGraph& Graph) override;
	virtual FVoxelRuntimeGraphData Translate(const UVoxelGraph& Graph) override;
	//~ End IVoxelGraphEditorCompiler Interface
};