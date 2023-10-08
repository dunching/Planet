// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphNodeVariable.h"
#include "SCommentBubble.h"

void SVoxelGraphNodeVariable::Construct(const FArguments& InArgs, UVoxelGraphNode* InNode)
{
	GraphNode = InNode;
	NodeDefinition = GetVoxelNode().GetNodeDefinition();

	SetCursor( EMouseCursor::CardinalCross );

	UpdateGraphNode();
}

void SVoxelGraphNodeVariable::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	SetupErrorReporting();

	ContentScale.Bind( this, &SGraphNode::GetContentScale );

	const FText NodeTitle = GraphNode->GetNodeTitle(ENodeTitleType::EditableTitle);

	FMargin ContentPadding(2.f, 0.f);
	if (NodeTitle.IsEmpty())
	{
		ContentPadding.Top = 2.f;
		ContentPadding.Bottom = 2.f;
	}
	else
	{
		ContentPadding.Top = 20.f;
		if (UVoxelGraphNode* Node = Cast<UVoxelGraphNode>(GraphNode))
		{
			if (Node->HasExecutionFlow())
			{
				ContentPadding.Top = 7.f;
			}
		}
	}

	GetOrAddSlot(ENodeZone::Center)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.VarNode.Body"))
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.VarNode.ColorSpill"))
				.ColorAndOpacity(this, &SVoxelGraphNodeVariable::GetVariableColor)
			]
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.VarNode.Gloss"))
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			.Padding(0.f, 8.f)
			[
				UpdateTitleWidget(NodeTitle)
			]
			+ SOverlay::Slot()
			.Padding(1.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.FillWidth(1.0f)
				.Padding(ContentPadding)
				[
					SAssignNew(LeftNodeBox, SVerticalBox)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.Padding(ContentPadding)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Top)
		.AutoHeight()
		.Padding(FMargin(5.0f, 1.0f))
		[
			ErrorReporting->AsWidget()
		]
	];

	{
		TSharedPtr<SCommentBubble> CommentBubble;

		SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

		GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];
	}

	CreatePinWidgets();

	// Align single pin widgets vertically on both sides
	if (LeftNodeBox->NumSlots() == RightNodeBox->NumSlots() &&
		LeftNodeBox->NumSlots() == 1)
	{
		RightNodeBox->GetSlot(RightNodeBox->NumSlots() - 1).SetFillHeight(1.f);
	}

	CreateInputSideAddButton(LeftNodeBox);
	CreateOutputSideAddButton(RightNodeBox);
}

const FSlateBrush* SVoxelGraphNodeVariable::GetShadowBrush(bool bSelected) const
{
	return bSelected ? FAppStyle::GetBrush(TEXT("Graph.VarNode.ShadowSelected")) : FAppStyle::GetBrush(TEXT("Graph.VarNode.Shadow"));
}

FSlateColor SVoxelGraphNodeVariable::GetVariableColor() const
{
	return GraphNode->GetNodeTitleColor();
}

TSharedRef<SWidget> SVoxelGraphNodeVariable::UpdateTitleWidget(FText InTitleText) const
{
	if (InTitleText.IsEmpty())
	{
		return SNullWidget::NullWidget;
	}

	return
		SNew(STextBlock)
		.TextStyle(FAppStyle::Get(), "Graph.Node.NodeTitle")
		.Text(InTitleText);
}