// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "SVoxelBlueprintGraphNode.h"
#include "K2Node_QueryVoxelChannel.h"

VOXEL_DEFAULT_MODULE(VoxelBlueprintEditor);

class FVoxelBlueprintGraphNodeFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* InNode) const override
	{
		VOXEL_FUNCTION_COUNTER();

		if (UK2Node_VoxelBaseNode* Node = Cast<UK2Node_VoxelBaseNode>(InNode))
		{
			return SNew(SVoxelBlueprintGraphNode, Node);
		}

		return nullptr;
	}
};

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterGraphConnectionDrawingPolicyFactory)
{
	FEdGraphUtilities::RegisterVisualNodeFactory(MakeShared<FVoxelBlueprintGraphNodeFactory>());
}