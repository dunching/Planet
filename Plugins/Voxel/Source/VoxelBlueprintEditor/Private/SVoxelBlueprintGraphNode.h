// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "KismetNodes/SGraphNodeK2Base.h"
#include "K2Node_VoxelBaseNode.h"

class UK2Node_VoxelBaseNode;

class SVoxelBlueprintGraphNode : public SGraphNodeK2Base
{
public:
	VOXEL_SLATE_ARGS()
	{};

	void Construct(const FArguments& InArgs, UK2Node_VoxelBaseNode* InNode);

	//~ Begin SGraphNodeK2Base interface
	virtual void CreatePinWidgets() override;
	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
	//~ End SGraphNodeK2Base interface

private:
	UK2Node_VoxelBaseNode& GetNode() const
	{
		return *CastChecked<UK2Node_VoxelBaseNode>(GraphNode);
	}
};