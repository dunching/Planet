// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphLocalVariableNode.h"
#include "VoxelGraphToolkit.h"

void UVoxelGraphLocalVariableDeclarationNode::AllocateParameterPins(const FVoxelGraphParameter& Parameter)
{
	{
		UEdGraphPin* Pin = CreatePin(EGPD_Input, Parameter.Type.GetEdGraphPinType(), FName("InputPin"));
		Pin->bAllowFriendlyName = true;
		Pin->PinFriendlyName = FText::FromName(Parameter.Name);
	}

	{
		UEdGraphPin* Pin = CreatePin(EGPD_Output, Parameter.Type.GetEdGraphPinType(), FName("OutputPin"));
		Pin->bAllowFriendlyName = true;
		Pin->PinFriendlyName = INVTEXT(" ");
	}
}

FText UVoxelGraphLocalVariableDeclarationNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType != ENodeTitleType::FullTitle)
	{
		return INVTEXT("LOCAL");
	}

	return FText::FromName(GetParameterSafe().Name);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraphLocalVariableUsageNode::AllocateParameterPins(const FVoxelGraphParameter& Parameter)
{
	UEdGraphPin* Pin = CreatePin(EGPD_Output, Parameter.Type.GetEdGraphPinType(), FName("OutputPin"));
	Pin->PinFriendlyName = FText::FromName(Parameter.Name);
}

FText UVoxelGraphLocalVariableUsageNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType != ENodeTitleType::FullTitle)
	{
		return {};
	}

	return FText::FromName(GetParameterSafe().Name);
}

void UVoxelGraphLocalVariableUsageNode::JumpToDefinition() const
{
	UVoxelGraphLocalVariableDeclarationNode* DeclarationNode = FindDeclaration();
	if (!DeclarationNode)
	{
		return;
	}

	const TSharedPtr<SGraphEditor> ActiveGraphEditor = GetToolkit()->GetActiveGraphEditor();
	ActiveGraphEditor->ClearSelectionSet();
	ActiveGraphEditor->SetNodeSelection(DeclarationNode, true);
	ActiveGraphEditor->ZoomToFit(true);
}

UVoxelGraphLocalVariableDeclarationNode* UVoxelGraphLocalVariableUsageNode::FindDeclaration() const
{
	TArray<UVoxelGraphLocalVariableDeclarationNode*> DeclarationNodes;
	GetGraph()->GetNodesOfClass<UVoxelGraphLocalVariableDeclarationNode>(DeclarationNodes);

	for (UVoxelGraphLocalVariableDeclarationNode* Node : DeclarationNodes)
	{
		if (Node->Guid == Guid)
		{
			return Node;
		}
	}

	return nullptr;
}