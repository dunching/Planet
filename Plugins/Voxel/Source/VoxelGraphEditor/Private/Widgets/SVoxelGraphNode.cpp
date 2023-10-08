// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphNode.h"
#include "VoxelNode.h"
#include "SVoxelToolTip.h"
#include "SCommentBubble.h"
#include "SLevelOfDetailBranchNode.h"

VOXEL_INITIALIZE_STYLE(GraphNodeEditor)
{
	Set("Node.Overlay.Warning", new IMAGE_BRUSH("Graphs/NodeOverlay_Warning", CoreStyleConstants::Icon32x32));
	Set("Node.Overlay.Debug", new CORE_IMAGE_BRUSH_SVG("Starship/Blueprints/Breakpoint_Valid", FVector2D(20.0f, 20.0f), FSlateColor(FColor::Cyan)));
	Set("Node.Overlay.Preview", new CORE_IMAGE_BRUSH_SVG("Starship/Blueprints/Breakpoint_Valid", FVector2D(20.0f, 20.0f), FSlateColor(FColor::Red)));

	Set("Pin.Buffer.Connected", new IMAGE_BRUSH("Graphs/BufferPin_Connected", FVector2D(15, 11)));
	Set("Pin.Buffer.Disconnected", new IMAGE_BRUSH("Graphs/BufferPin_Disconnected", FVector2D(15, 11)));
	Set("Pin.Buffer.Promotable.Inner", new IMAGE_BRUSH_SVG("Graphs/BufferPin_Promotable_Inner", CoreStyleConstants::Icon14x14));
	Set("Pin.Buffer.Promotable.Outer", new IMAGE_BRUSH_SVG("Graphs/BufferPin_Promotable_Outer", CoreStyleConstants::Icon14x14));

	Set("Pin.PointSet.Connected", new IMAGE_BRUSH("Graphs/PointSetPin_Connected", FVector2D(19, 15)));
	Set("Pin.PointSet.Disconnected", new IMAGE_BRUSH("Graphs/PointSetPin_Disconnected", FVector2D(19, 15)));

	Set("Pin.Surface.Connected", new IMAGE_BRUSH("Graphs/SurfacePin_Connected", FVector2D(19, 15)));
	Set("Pin.Surface.Disconnected", new IMAGE_BRUSH("Graphs/SurfacePin_Disconnected", FVector2D(19, 15)));

	Set("Node.Stats.TitleGloss", new BOX_BRUSH("Graphs/Node_Stats_Gloss", FMargin(12.0f / 64.0f)));
	Set("Node.Stats.ColorSpill", new BOX_BRUSH("Graphs/Node_Stats_Color_Spill", FMargin(8.0f / 64.0f, 3.0f / 32.0f, 0.f, 0.f)));

	Set("Icons.MinusCircle", new IMAGE_BRUSH_SVG("Graphs/Minus_Circle", CoreStyleConstants::Icon16x16));
}

void SVoxelGraphNode::Construct(const FArguments& Args, UVoxelGraphNode* InNode)
{
	VOXEL_FUNCTION_COUNTER();

	GraphNode = InNode;
	NodeDefinition = GetVoxelNode().GetNodeDefinition();

	SetCursor(EMouseCursor::CardinalCross);
	SetToolTip(
		SNew(SVoxelToolTip)
		.Text(GetNodeTooltip()));

	ContentScale.Bind(this, &SGraphNode::GetContentScale);

	UpdateGraphNode();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	LeftNodeBox.Reset();
	RightNodeBox.Reset();

	CategoryPins.Reset();

	SetupErrorReporting();

	if (GetVoxelNode().IsCompact())
	{
		UpdateCompactNode();
	}
	else
	{
		UpdateStandardNode();
	}

	FString Type;
	FString Tooltip;
	FString ColorType;
	if (GetVoxelNode().GetOverlayInfo(Type, Tooltip, ColorType))
	{
		const FSlateBrush* ImageBrush;
		if (Type == "Warning")
		{
			ImageBrush = FVoxelEditorStyle::Get().GetBrush(TEXT("Node.Overlay.Warning"));
		}
		else if (Type == "Lighting")
		{
			ImageBrush = FAppStyle::Get().GetBrush(TEXT("Graph.AnimationFastPathIndicator"));
		}
		else if (Type == "Clock")
		{
			ImageBrush = FAppStyle::Get().GetBrush(TEXT("Graph.Latent.LatentIcon"));
		}
		else if (Type == "Message")
		{
			ImageBrush = FAppStyle::Get().GetBrush(TEXT("Graph.Message.MessageIcon"));
		}
		else
		{
			ensure(false);
			return;
		}

		FLinearColor Color = FLinearColor::White;
		if (ColorType == "Red")
		{
			Color = FLinearColor::Red;
		}

		OverlayWidget.Widget =
			SNew(SImage)
			.Image(ImageBrush)
			.ToolTipText(FText::FromString(Tooltip))
			.Visibility(EVisibility::Visible)
			.ColorAndOpacity(Color);

		OverlayWidget.BrushSize = ImageBrush->ImageSize;
	}
}

void SVoxelGraphNode::CreateOutputSideAddButton(TSharedPtr<SVerticalBox> OutputBox)
{
	const TSharedRef<SWidget> Button = AddPinButtonContent(FText::FromString(NodeDefinition->GetAddPinLabel()), {});
	Button->SetCursor(EMouseCursor::Default);

	FMargin Padding = Settings->GetOutputPinPadding();
	Padding.Top += 6.0f;

	OutputBox->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(Padding)
		[
			SNew(SBox)
			.Padding(0)
			.Visibility(this, &SVoxelGraphNode::IsAddPinButtonVisible)
			.ToolTipText(FText::FromString(NodeDefinition->GetAddPinTooltip()))
			[
				Button
			]
		];
}

EVisibility SVoxelGraphNode::IsAddPinButtonVisible() const
{
	return GetButtonVisibility(NodeDefinition->CanAddInputPin());
}

FReply SVoxelGraphNode::OnAddPin()
{
	const FVoxelTransaction Transaction(GetVoxelNode(), "Add input pin");
	NodeDefinition->AddInputPin();
	return FReply::Handled();
}

void SVoxelGraphNode::RequestRenameOnSpawn()
{
	if (GetVoxelNode().CanRenameOnSpawn())
	{
		RequestRename();
	}
}

TArray<FOverlayWidgetInfo> SVoxelGraphNode::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;

	if (OverlayWidget.Widget)
	{
		FOverlayWidgetInfo Info;
		Info.OverlayOffset = OverlayWidget.GetLocation(WidgetSize);
		Info.Widget = OverlayWidget.Widget;

		Widgets.Add(Info);
	}

	return Widgets;
}

void SVoxelGraphNode::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	if (GetVoxelNode().bEnableDebug)
	{
		const FSlateBrush* Brush = FVoxelEditorStyle::Get().GetBrush(TEXT("Node.Overlay.Debug"));

		FOverlayBrushInfo BrushInfo;
		BrushInfo.Brush = Brush;
		BrushInfo.OverlayOffset = -Brush->GetImageSize() / 2.f;
		Brushes.Add(BrushInfo);
	}

	if (GetVoxelNode().bEnablePreview)
	{
		const FSlateBrush* Brush = FVoxelEditorStyle::Get().GetBrush(TEXT("Node.Overlay.Preview"));

		FOverlayBrushInfo BrushInfo;
		BrushInfo.Brush = Brush;
		BrushInfo.OverlayOffset = -Brush->GetImageSize() / 2.f;
		Brushes.Add(BrushInfo);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphNode::UpdateStandardNode()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	const FSlateBrush* IconBrush;
	if (GraphNode &&
		GraphNode->ShowPaletteIconOnNode())
	{
		IconColor = FLinearColor::White;
		IconBrush = GraphNode->GetIconAndTint(IconColor).GetOptionalIcon();
	}
	else
	{
		IconColor = FLinearColor::White;
		IconBrush = nullptr;
	}

	TSharedPtr<SHorizontalBox> TitleBox;

	const TSharedRef<SWidget> TitleAreaWidget =
		SNew(SLevelOfDetailBranchNode)
		.UseLowDetailSlot(this, &SVoxelGraphNode::UseLowDetailNodeTitles)
		.LowDetail()
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.Node.ColorSpill"))
			.Padding(FMargin(75.0f, 22.0f)) // Saving enough space for a 'typical' title so the transition isn't quite so abrupt
			.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
		]
		.HighDetail()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.Node.TitleGloss"))
				.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SAssignNew(TitleBox, SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.AutoWidth()
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Graph.Node.ColorSpill"))
					// The extra margin on the right is for making the color spill stretch well past the node title
					.Padding(FMargin(10, 5, 30, 3))
					.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleColor)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Top)
						.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
						.AutoWidth()
						[
							SNew(SImage)
							.Image(IconBrush)
							.ColorAndOpacity(this, &SGraphNode::GetNodeTitleIconColor)
						]
						+ SHorizontalBox::Slot()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								CreateTitleWidget(NodeTitle)
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle
							]
						]
					]
				]
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			[
				SNew(SBorder)
				.Visibility(EVisibility::HitTestInvisible)
				.BorderImage(FAppStyle::GetBrush("Graph.Node.TitleHighlight"))
				.BorderBackgroundColor(this, &SGraphNode::GetNodeTitleIconColor)
				[
					SNew(SSpacer)
					.Size(FVector2D(20, 20))
				]
			]
		];

	if (TitleBox)
	{
		CreateStandardNodeTitleButtons(TitleBox);
	}

	TSharedPtr<SVerticalBox> InnerVerticalBox;
	GetOrAddSlot(ENodeZone::Center)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.Padding(Settings->GetNonPinNodeBodyPadding())
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.Node.Body"))
				.ColorAndOpacity(this, &SVoxelGraphNode::GetNodeBodyColor)
			]
			+ SOverlay::Slot()
			[
				SAssignNew(InnerVerticalBox, SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				.Padding(Settings->GetNonPinNodeBodyPadding())
				[
					TitleAreaWidget
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					CreateNodeContentArea()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(Settings->GetNonPinNodeBodyPadding())
				[
					MakeStatWidget()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					ErrorReporting->AsWidget()
				]
			]
		]
	];

	const TSharedRef<SCommentBubble> CommentBubble =
		SNew(SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.OnToggled(this, &SGraphNode::OnCommentBubbleToggled)
		.ColorAndOpacity(GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
	.SlotOffset(MakeAttributeSP(&CommentBubble.Get(), &SCommentBubble::GetOffset))
	.SlotSize(MakeAttributeSP(&CommentBubble.Get(), &SCommentBubble::GetSize))
	.AllowScaling(MakeAttributeSP(&CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
	.VAlign(VAlign_Top)
	[
		CommentBubble
	];

	if (NodeDefinition->OverridePinsOrder())
	{
		CreateCategorizedPinWidgets();
	}
	else
	{
		CreatePinWidgets();
	}

	CreateInputSideAddButton(LeftNodeBox);
	UpdateBottomContent(InnerVerticalBox);
	CreateAdvancedCategory(InnerVerticalBox);
}

void SVoxelGraphNode::UpdateCompactNode()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	constexpr float MinNodePadding = 55.f;
	constexpr float MaxNodePadding = 180.0f;
	constexpr float PaddingIncrementSize = 20.0f;

	const float PinPaddingRight = FMath::Clamp(MinNodePadding + NodeTitle->GetHeadTitle().ToString().Len() * PaddingIncrementSize, MinNodePadding, MaxNodePadding);

	const TSharedRef<SOverlay> NodeOverlay =
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, PinPaddingRight, 0.f)
		[
			// LEFT
			SAssignNew(LeftNodeBox, SVerticalBox)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(45.f, 0.f, 45.f, 0.f)
		[
			// MIDDLE
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FAppStyle::Get(), "Graph.CompactNode.Title")
				.Text(&NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
				.WrapTextAt(128.0f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				NodeTitle
			]
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(55.f, 0.f, 0.f, 0.f)
		[
			// RIGHT
			SAssignNew(RightNodeBox, SVerticalBox)
		];

	this->GetOrAddSlot(ENodeZone::Center)
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
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.VarNode.Gloss"))
			]
			+ SOverlay::Slot()
			.Padding(FMargin(0.f, 3.f, 0.f, 0.f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(FMargin(0.f, 0.f, 0.f, 4.f))
				[
					NodeOverlay
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					MakeStatWidget()
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			ErrorReporting->AsWidget()
		]
	];

	CreatePinWidgets();

	// Hide pin labels
	for (const TSharedRef<SGraphPin>& Pin : InputPins)
	{
		if (!Pin->GetPinObj()->ParentPin)
		{
			Pin->SetShowLabel(false);
		}
	}
	for (const TSharedRef<SGraphPin>& Pin : OutputPins)
	{
		if (!Pin->GetPinObj()->ParentPin)
		{
			Pin->SetShowLabel(false);
		}
	}

	const TSharedRef<SCommentBubble> CommentBubble =
		SNew(SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SVoxelGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
	.SlotOffset(MakeAttributeSP(&CommentBubble.Get(), &SCommentBubble::GetOffset))
	.SlotSize(MakeAttributeSP(&CommentBubble.Get(), &SCommentBubble::GetSize))
	.AllowScaling(MakeAttributeSP(&CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
	.VAlign(VAlign_Top)
	[
		CommentBubble
	];

	CreateInputSideAddButton(LeftNodeBox);
	CreateOutputSideAddButton(RightNodeBox);
}

///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphNode::CreateCategorizedPinWidgets()
{
	TArray<UEdGraphPin*> Pins = GraphNode->Pins;

	TMap<FName, TArray<UEdGraphPin*>> MappedSplitPins;
	for (UEdGraphPin* Pin : Pins)
	{
		if (!Pin->ParentPin)
		{
			continue;
		}

		MappedSplitPins.FindOrAdd(Pin->ParentPin->PinName, {}).Add(Pin);
	}

	for (const auto& It : MappedSplitPins)
	{
		for (UEdGraphPin* Pin : It.Value)
		{
			Pins.RemoveSwap(Pin);
		}
	}

	if (const TSharedPtr<const IVoxelNodeDefinition::FNode> Inputs = NodeDefinition->GetInputs())
	{
		for (const TSharedRef<IVoxelNodeDefinition::FNode>& Node : Inputs->Children)
		{
			CreateCategoryPinWidgets(Node, Pins, MappedSplitPins, LeftNodeBox, true);
		}
	}

	if (const TSharedPtr<const IVoxelNodeDefinition::FNode> Outputs = NodeDefinition->GetOutputs())
	{
		for (const TSharedRef<IVoxelNodeDefinition::FNode>& Node : Outputs->Children)
		{
			CreateCategoryPinWidgets(Node, Pins, MappedSplitPins, RightNodeBox, false);
		}
	}

	for (UEdGraphPin* Pin : Pins)
	{
		ensure(Pin->bOrphanedPin);
		AddStandardNodePin(
			Pin,
			{},
			{},
			Pin->Direction == EGPD_Input ? LeftNodeBox : RightNodeBox);
	}
}

void SVoxelGraphNode::CreateCategoryPinWidgets(const TSharedRef<IVoxelNodeDefinition::FNode>& Node, TArray<UEdGraphPin*>& Pins, TMap<FName, TArray<UEdGraphPin*>>& MappedSplitPins, const TSharedPtr<SVerticalBox>& TargetContainer, const bool bInput)
{
	if (Node->NodeState == IVoxelNodeDefinition::ENodeState::Pin)
	{
		if (const auto* PinPtr = Pins.FindByPredicate([&Node](const UEdGraphPin* Pin)
		{
			return Node->Name == Pin->PinName;
		}))
		{
			if (UEdGraphPin* Pin = *PinPtr)
			{
				AddStandardNodePin(Pin, Node->GetFullPath(), Node->Path, TargetContainer);
				Pins.RemoveSwap(Pin);
			}
		}
		if (const auto SplitPinsPtr = MappedSplitPins.Find(Node->Name))
		{
			for (UEdGraphPin* SplitPin : *SplitPinsPtr)
			{
				AddStandardNodePin(SplitPin, Node->GetFullPath(), Node->Path, TargetContainer);
			}

			MappedSplitPins.Remove(Node->Name);
		}
		return;
	}

	const TSharedPtr<SVerticalBox> PinsBox = CreateCategoryWidget(Node->Name, Node->GetFullPath(), Node->Path, Node->Children.Num(), bInput, Node->NodeState == IVoxelNodeDefinition::ENodeState::ArrayCategory);
	if (TargetContainer != PinsBox)
	{
		TargetContainer->AddSlot()
		.Padding(bInput ? FMargin(Settings->GetInputPinPadding().Left, 0.f, 0.f, 0.f) : FMargin(0.f, 0.f, Settings->GetOutputPinPadding().Right, 0.f))
		.AutoHeight()
		.HAlign(bInput ? HAlign_Left : HAlign_Right)
		.VAlign(VAlign_Center)
		[
			PinsBox.ToSharedRef()
		];
	}

	for (const TSharedRef<IVoxelNodeDefinition::FNode>& InnerNode : Node->Children)
	{
		CreateCategoryPinWidgets(InnerNode, Pins, MappedSplitPins, PinsBox, bInput);
	}
}

void SVoxelGraphNode::AddStandardNodePin(UEdGraphPin* PinToAdd, const FName FullPath, const TArray<FName>& Path, const TSharedPtr<SVerticalBox>& TargetContainer)
{
	if (!ensure(PinToAdd))
	{
		return;
	}

	// ShouldPinBeShown
	if (!ShouldPinBeHidden(PinToAdd))
	{
		return;
	}

	const TSharedPtr<SGraphPin> NewPin = CreatePinWidget(PinToAdd);
	if (!ensure(NewPin.IsValid()))
	{
		return;
	}
	const TSharedRef<SGraphPin> PinWidget = NewPin.ToSharedRef();

	PinWidget->SetOwner(SharedThis(this));

	if (Path.Num() > 0)
	{
		CategoryPins.FindOrAdd(FullPath).Add(PinWidget);
		PinWidget->SetVisibility(MakeAttributeLambda([=]
		{
			if (PinToAdd->bHidden ||
				!NodeDefinition->IsPinVisible(PinToAdd, GraphNode))
			{
				return EVisibility::Collapsed;
			}

			const TSet<FName>& CollapsedCategories = PinToAdd->Direction == EGPD_Input ? GetVoxelNode().CollapsedInputCategories : GetVoxelNode().CollapsedOutputCategories;
			bool bIsCollapsed = false;

			FString Category = Path[0].ToString();
			if (Category == "Advanced")
			{
				bIsCollapsed = !GetVoxelNode().bShowAdvanced;
			}
			else
			{
				bIsCollapsed = CollapsedCategories.Contains(FName(Category));
			}

			for (int32 Index = 1; Index < Path.Num() && !bIsCollapsed; Index++)
			{
				Category += "|" + Path[Index].ToString();

				bIsCollapsed = CollapsedCategories.Contains(FName(Category));
			}

			return
				bIsCollapsed &&
				PinToAdd->LinkedTo.Num() == 0
				? EVisibility::Collapsed
				: EVisibility::Visible;
		}));
	}
	else
	{
		PinWidget->SetVisibility(MakeAttributeLambda([=]
		{
			if (PinToAdd->bHidden ||
				!NodeDefinition->IsPinVisible(PinToAdd, GraphNode))
			{
				return EVisibility::Collapsed;
			}
			return EVisibility::Visible;
		}));
	}

	TargetContainer->AddSlot()
		.AutoHeight()
		.HAlign(PinToAdd->Direction == EGPD_Input ? HAlign_Left : HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(PinToAdd->Direction == EGPD_Input ? Settings->GetInputPinPadding() : Settings->GetOutputPinPadding())
		[
			PinWidget
		];
	(PinToAdd->Direction == EGPD_Input ? InputPins : OutputPins).Add(PinWidget);
}

TSharedRef<SVerticalBox> SVoxelGraphNode::CreateCategoryWidget(const FName Name, const FName FullPath, const TArray<FName>& Path, const int32 ArrayNum, const bool bIsInput, const bool bIsArrayCategory)
{
	if (FullPath == STATIC_FNAME("Advanced"))
	{
		bCreateAdvancedCategory = true;
		return bIsInput ? LeftNodeBox.ToSharedRef() : RightNodeBox.ToSharedRef();
	}

	const TSharedRef<SButton> Button =
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), TEXT("SimpleButton"))
		.ContentPadding(0)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.ClickMethod(EButtonClickMethod::MouseDown)
		.OnClicked_Lambda([=]
		{
			TSet<FName>& CollapsedCategories = bIsInput ? GetVoxelNode().CollapsedInputCategories : GetVoxelNode().CollapsedOutputCategories;
			if (CollapsedCategories.Contains(FullPath))
			{
				CollapsedCategories.Remove(FullPath);
			}
			else
			{
				CollapsedCategories.Add(FullPath);
			}

			return FReply::Handled();
		});

	Button->SetContent(
		SNew(SImage)
		.Image_Lambda([=, ButtonPtr = &Button.Get()]() -> const FSlateBrush*
		{
			const TSet<FName>& CollapsedCategories = bIsInput ? GetVoxelNode().CollapsedInputCategories : GetVoxelNode().CollapsedOutputCategories;
			if (CollapsedCategories.Contains(FullPath))
			{
				if (ButtonPtr->IsHovered())
				{
					return FAppStyle::Get().GetBrush(STATIC_FNAME("TreeArrow_Collapsed_Hovered"));
				}
				else
				{
					return FAppStyle::Get().GetBrush(STATIC_FNAME("TreeArrow_Collapsed"));
				}
			}
			else
			{
				if (ButtonPtr->IsHovered())
				{
					return FAppStyle::Get().GetBrush(STATIC_FNAME("TreeArrow_Expanded_Hovered"));
				}
				else
				{
					return FAppStyle::Get().GetBrush(STATIC_FNAME("TreeArrow_Expanded"));
				}
			}
		})
		.ColorAndOpacity(FSlateColor::UseForeground()));

	const FString CategoryDisplayName = FName::NameToDisplayString(Name.ToString(), false);
	const TSharedRef<SHorizontalBox> CategoryNameWidget =
		SNew(SHorizontalBox)
		.Cursor(EMouseCursor::Default);
	if (bIsInput)
	{
		CategoryNameWidget->AddSlot()
		.AutoWidth()
		[
			Button
		];
		CategoryNameWidget->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SVoxelDetailText)
			.Text(FText::FromString(CategoryDisplayName))
		];
	}

	TSharedRef<SVerticalBox> Result = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.HAlign(bIsInput ? HAlign_Left : HAlign_Right)
	[
		CategoryNameWidget
	];

	bool bAddVisibilityCheck = true;
	if (bIsArrayCategory)
	{
		CategoryNameWidget->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(3.f, 0.f, 0.f, 0.f)
		[
			SNew(SVoxelDetailText)
			.Text(FText::FromString("[" + LexToString(ArrayNum) + "]"))
			.ColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f))
		];

		CategoryNameWidget->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(5.f, 0.f, 0.f, 0.f)
		[
			SNew(SButton)
			.ContentPadding(0.0f)
			.Cursor(EMouseCursor::Default)
			.ToolTipText(INVTEXT("Remove pin"))
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.IsEnabled_Lambda([this, Name]
			{
				return IsNodeEditable() && NodeDefinition->CanRemoveFromCategory(Name);
			})
			.OnClicked_Lambda([this, Name]
			{
				const FVoxelTransaction Transaction(GetVoxelNode(), "Remove pin");
				NodeDefinition->RemoveFromCategory(Name);
				return FReply::Handled();
			})
			.Visibility_Lambda([this, Name]
			{
				return GetButtonVisibility(NodeDefinition->CanAddToCategory(Name) || NodeDefinition->CanRemoveFromCategory(Name));
			})
			[
				SNew(SImage)
				.Image(FVoxelEditorStyle::GetBrush(TEXT("Icons.MinusCircle")))
			]
		];

		CategoryNameWidget->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(2.f, 0.f, 0.f, 0.f)
		[
			SNew(SButton)
			.ContentPadding(0.0f)
			.Cursor(EMouseCursor::Default)
			.ToolTipText(INVTEXT("Add pin"))
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.IsEnabled_Lambda([=]
			{
				return IsNodeEditable() && NodeDefinition->CanAddToCategory(Name);
			})
			.OnClicked_Lambda([=]
			{
				const FVoxelTransaction Transaction(GetVoxelNode(), "Add pin");
				NodeDefinition->AddToCategory(Name);
				return FReply::Handled();
			})
			.Visibility_Lambda([=]
			{
				return GetButtonVisibility(NodeDefinition->CanAddToCategory(Name) || NodeDefinition->CanRemoveFromCategory(Name));
			})
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
			]
		];

		if (ArrayNum == 0)
		{
			const FMargin Padding = Settings->GetInputPinPadding();

			Result->AddSlot()
			.Padding(Padding + FMargin(Padding.Left, 0.f, 0.f, 0.f))
			.AutoHeight()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SVoxelDetailText)
				.Visibility_Lambda([=]
				{
					return GetVoxelNode().CollapsedInputCategories.Contains(FullPath) ? EVisibility::Collapsed : EVisibility::Visible;
				})
				.Text(INVTEXT("No entries"))
				.ColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f))
			];

			if (Path.Num() > 1)
			{
				bAddVisibilityCheck = false;
				Result->SetVisibility(MakeAttributeLambda([=]
				{
					const TSet<FName>& CollapsedCategories = bIsInput ? GetVoxelNode().CollapsedInputCategories : GetVoxelNode().CollapsedOutputCategories;

					FString Category = Path[0].ToString();

					if (Category == "Advanced")
					{
						if (!GetVoxelNode().bShowAdvanced)
						{
							return EVisibility::Collapsed;
						}
					}
					else if (CollapsedCategories.Contains(FName(Category)))
					{
						return EVisibility::Collapsed;
					}

					for (int32 Index = 1; Index < Path.Num() - 1; Index++)
					{
						Category += "|" + Path[Index].ToString();
						if (CollapsedCategories.Contains(FName(Category)))
						{
							return EVisibility::Collapsed;
						}
					}

					return EVisibility::Visible;
				}));
			}
		}
	}

	if (bAddVisibilityCheck &&
		Path.Num() > 1)
	{
		Result->SetVisibility(MakeAttributeLambda([=]
		{
			const TSet<FName>& CollapsedCategories = bIsInput ? GetVoxelNode().CollapsedInputCategories : GetVoxelNode().CollapsedOutputCategories;

			bool bIsCollapsed = false;

			FString Category = Path[0].ToString();
			if (Category == "Advanced")
			{
				bIsCollapsed = !GetVoxelNode().bShowAdvanced;
			}
			else
			{
				bIsCollapsed = CollapsedCategories.Contains(FName(Category));
			}

			for (int32 Index = 1; Index < Path.Num() - 1 && !bIsCollapsed; Index++)
			{
				Category += "|" + Path[Index].ToString();

				bIsCollapsed = CollapsedCategories.Contains(FName(Category));
			}

			if (!bIsCollapsed)
			{
				return EVisibility::Visible;
			}

			const TArray<TWeakPtr<SGraphPin>>* PinsPtr = CategoryPins.Find(FullPath);
			if (!ensure(PinsPtr))
			{
				return EVisibility::Collapsed;
			}
			else
			{
				for (const TWeakPtr<SGraphPin>& Pin : *PinsPtr)
				{
					const TSharedPtr<SGraphPin> PinnedPin = Pin.Pin();
					if (!PinnedPin)
					{
						continue;
					}

					const UEdGraphPin* PinObject = PinnedPin->GetPinObj();
					if (!PinObject)
					{
						continue;
					}

					if (PinObject->LinkedTo.Num() > 0)
					{
						return EVisibility::Visible;
					}
				}

				return EVisibility::Collapsed;
			}
		}));
	}

	if (!bIsInput)
	{
		CategoryNameWidget->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SVoxelDetailText)
			.Text(FText::FromString(CategoryDisplayName))
		];
		CategoryNameWidget->AddSlot()
		.AutoWidth()
		[
			Button
		];
	}

	return Result;
}

void SVoxelGraphNode::CreateAdvancedCategory(const TSharedPtr<SVerticalBox>& MainBox) const
{
	if (!bCreateAdvancedCategory)
	{
		return;
	}

	MainBox->AddSlot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Top)
	.Padding(3.f, 0.f, 3.f, 3.f)
	[
		SNew(SCheckBox)
		.OnCheckStateChanged_Lambda([this](ECheckBoxState)
		{
			GetVoxelNode().bShowAdvanced = !GetVoxelNode().bShowAdvanced;
		})
		.IsChecked_Lambda([this]
		{
			return GetVoxelNode().bShowAdvanced ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		})
		.Cursor(EMouseCursor::Default)
		.Style(FAppStyle::Get(), "Graph.Node.AdvancedView")
		.ToolTipText_Lambda([this]
		{
			return GetVoxelNode().bShowAdvanced ? INVTEXT("Hide advanced pins") : INVTEXT("Show advanced pins");
		})
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SImage)
				.Image_Lambda([this]
				{
					return FAppStyle::GetBrush(GetVoxelNode().bShowAdvanced ? "Icons.ChevronUp" : "Icons.ChevronDown");
				})
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphNode::MakeStatWidget() const
{
	const FLinearColor Color = FLinearColor(FColor::Orange) * 0.6f;

	const TSharedRef<SVerticalBox> VBox =
		SNew(SVerticalBox)
		.Cursor(EMouseCursor::Default);

	for (IVoxelNodeStatProvider* Provider : GVoxelNodeStatProviders)
	{
		VBox->AddSlot()
		.AutoHeight()
		[
			SNew(SOverlay)
			.ToolTipText_Lambda([this, Provider]
			{
				return Provider->GetToolTip(GetVoxelNode());
			})
			.Visibility_Lambda([this, Provider]
			{
				if (!GVoxelEnableNodeStats)
				{
					return EVisibility::Collapsed;
				}

				if (Provider->GetText(GetVoxelNode()).IsEmpty())
				{
					return EVisibility::Collapsed;
				}

				return EVisibility::Visible;
			})
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FVoxelEditorStyle::GetBrush("Node.Stats.TitleGloss"))
				.ColorAndOpacity(Color)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SBorder)
				.BorderImage(FVoxelEditorStyle::GetBrush("Node.Stats.ColorSpill"))
				.Padding(FMargin(10.f, 5.f, 20.f, 3.f))
				.BorderBackgroundColor(Color)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Top)
					.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
					.AutoWidth()
					[
						SNew(SImage)
						.Image(FAppStyle::GetBrush("GraphEditor.Timeline_16x"))
						.ColorAndOpacity(Color)
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SBox)
						.MinDesiredWidth(55.f)
						[
							SNew(STextBlock)
							.ColorAndOpacity(FCoreStyle::Get().GetColor("ErrorReporting.ForegroundColor"))
							.Text_Lambda([this, Provider]
							{
								return Provider->GetText(GetVoxelNode());
							})
						]
					]
				]
			]
		];
	}
	return VBox;
}

EVisibility SVoxelGraphNode::GetButtonVisibility(bool bVisible) const
{
	if (SGraphNode::IsAddPinButtonVisible() == EVisibility::Collapsed)
	{
		// LOD is too low
		return EVisibility::Collapsed;
	}

	return bVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

void SVoxelGraphNode::CreateStandardNodeTitleButtons(const TSharedPtr<SHorizontalBox>& TitleBox)
{
	TitleBox->AddSlot()
	.FillWidth(1.f)
	.HAlign(HAlign_Right)
	.VAlign(VAlign_Center)
	.Padding(0.f, 1.f, 7.f, 0.f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0.f, 0.f, 2.f, 0.f)
		[
			SNew(SButton)
			.ContentPadding(0.0f)
			.Cursor(EMouseCursor::Default)
			.IsEnabled_Lambda([this]
			{
				return IsNodeEditable() && NodeDefinition->CanRemoveInputPin();
			})
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.ToolTipText(FText::FromString(NodeDefinition->GetRemovePinTooltip()))
			.OnClicked_Lambda([this]
			{
				const FVoxelTransaction Transaction(GetVoxelNode(), "Remove input pin");
				NodeDefinition->RemoveInputPin();
				return FReply::Handled();
			})
			.Visibility_Lambda([this]
			{
				return GetButtonVisibility(NodeDefinition->CanAddInputPin() || NodeDefinition->CanRemoveInputPin());
			})
			[
				SNew(SImage)
				.Image(FVoxelEditorStyle::GetBrush(TEXT("Icons.MinusCircle")))
			]
		]
		+ SHorizontalBox::Slot()
		[
			SNew(SButton)
			.ContentPadding(0.0f)
			.Cursor(EMouseCursor::Default)
			.IsEnabled_Lambda([this]
			{
				return IsNodeEditable() && NodeDefinition->CanAddInputPin();
			})
			.ButtonStyle(FAppStyle::Get(), "NoBorder")
			.ToolTipText(FText::FromString(NodeDefinition->GetAddPinTooltip()))
			.OnClicked_Lambda([this]
			{
				const FVoxelTransaction Transaction(GetVoxelNode(), "Add input pin");
				NodeDefinition->AddInputPin();
				return FReply::Handled();
			})
			.Visibility_Lambda([this]
			{
				return GetButtonVisibility(NodeDefinition->CanAddInputPin() || NodeDefinition->CanRemoveInputPin());
			})
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush(TEXT("Icons.PlusCircle")))
			]
		]
	];
}