// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "SGraphNode.h"
#include "VoxelGraphNodeBase.h"

class SVoxelGraphNodeBase : public SGraphNode
{
public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& InArgs, UVoxelGraphNodeBase* InNode);

	UVoxelGraphNodeBase& GetVoxelBaseNode() const
	{
		return *CastChecked<UVoxelGraphNodeBase>(GraphNode);
	}

protected:
	//~ Begin SGraphNode Interface
	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override;
	//~ End SGraphNode Interface
};