// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

struct FVoxelGraphToolkit;

class FVoxelGraphParameterSelectionCustomization : public IDetailCustomization
{
public:
	const TWeakObjectPtr<UEdGraph> EdGraph;
	const FGuid TargetParameterGuid;
	const TWeakObjectPtr<UObject> SelectedNode;

	FVoxelGraphParameterSelectionCustomization(
		const TWeakObjectPtr<UEdGraph>& EdGraph,
		const FGuid TargetParameterId,
		const TWeakObjectPtr<UObject>& SelectedNode)
		: EdGraph(EdGraph)
		, TargetParameterGuid(TargetParameterId)
		, SelectedNode(SelectedNode)
	{
	}

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	//~ End IDetailCustomization Interface
};