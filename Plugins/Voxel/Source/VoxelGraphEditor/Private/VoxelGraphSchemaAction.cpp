// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphSchemaAction.h"
#include "VoxelGraphSchemaBase.h"
#include "VoxelGraphToolkit.h"
#include "EdGraphNode_Comment.h"

UEdGraphNode* FVoxelGraphSchemaAction_NewComment::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "Add comment");

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = UVoxelGraphSchemaBase::GetToolkit(ParentGraph);
	if (!ensure(Toolkit))
	{
		return nullptr;
	}

	const TSharedPtr<SGraphEditor> GraphEditor = Toolkit->GetActiveGraphEditor();
	if (!ensure(GraphEditor))
	{
		return nullptr;
	}

	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>();

	FVector2D SpawnLocation = Location;

	FSlateRect Bounds;
	if (GraphEditor->GetBoundsForSelectedNodes(Bounds, 50.f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation);
}

void FVoxelGraphSchemaAction_NewComment::GetIcon(FSlateIcon& Icon, FLinearColor& Color)
{
	static const FSlateIcon CommentIcon("EditorStyle", "Icons.Comment");
	Icon = CommentIcon;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UEdGraphNode* FVoxelGraphSchemaAction_Paste::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FVoxelTransaction Transaction(ParentGraph, "Paste nodes");

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = UVoxelGraphSchemaBase::GetToolkit(ParentGraph);
	if (ensure(Toolkit))
	{
		Toolkit->PasteNodesHere(Location);
	}

	return nullptr;
}