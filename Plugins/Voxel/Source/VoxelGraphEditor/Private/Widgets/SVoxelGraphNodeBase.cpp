// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphNodeBase.h"
#include "VoxelPinType.h"
#include "VoxelSurface.h"
#include "SVoxelToolTip.h"
#include "Point/VoxelPointSet.h"

void SVoxelGraphNodeBase::Construct(const FArguments& InArgs, UVoxelGraphNodeBase* InNode)
{
	VOXEL_FUNCTION_COUNTER();

	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();

	SetToolTip(
		SNew(SVoxelToolTip)
		.Text(GetNodeTooltip()));
}

TSharedPtr<SGraphPin> SVoxelGraphNodeBase::CreatePinWidget(UEdGraphPin* Pin) const
{
	const TSharedPtr<SGraphPin> GraphPin = SGraphNode::CreatePinWidget(Pin);
	if (!ensure(GraphPin))
	{
		return nullptr;
	}

	if (GetVoxelBaseNode().IsPinOptional(*Pin) &&
		!Pin->HasAnyConnections())
	{
		GraphPin->SetPinColorModifier(FLinearColor(1.f, 1.f, 1.f, 0.5f));
	}

	{
		SLayeredImage& PinImage = static_cast<SLayeredImage&>(*GraphPin->GetPinImageWidget());

		static const FSlateBrush* PromotableOuterIcon = FAppStyle::GetBrush("Kismet.VariableList.PromotableTypeOuterIcon");
		static const FSlateBrush* PromotableInnerIcon = FAppStyle::GetBrush("Kismet.VariableList.PromotableTypeInnerIcon");

		static const FSlateBrush* BufferConnectedIcon = FVoxelEditorStyle::GetBrush("Pin.Buffer.Connected");
		static const FSlateBrush* BufferDisconnectedIcon = FVoxelEditorStyle::GetBrush("Pin.Buffer.Disconnected");
		static const FSlateBrush* BufferPromotableInnerIcon = FVoxelEditorStyle::GetBrush("Pin.Buffer.Promotable.Inner");
		static const FSlateBrush* BufferPromotableOuterIcon = FVoxelEditorStyle::GetBrush("Pin.Buffer.Promotable.Outer");

		static const FSlateBrush* PointSetConnectedIcon = FVoxelEditorStyle::GetBrush("Pin.PointSet.Connected");
		static const FSlateBrush* PointSetDisconnectedIcon = FVoxelEditorStyle::GetBrush("Pin.PointSet.Disconnected");

		static const FSlateBrush* SurfaceConnectedIcon = FVoxelEditorStyle::GetBrush("Pin.Surface.Connected");
		static const FSlateBrush* SurfaceDisconnectedIcon = FVoxelEditorStyle::GetBrush("Pin.Surface.Disconnected");

		static const FSlateBrush* ArrayConnectedIcon = FAppStyle::GetBrush("Graph.ArrayPin.Connected");
		static const FSlateBrush* ArrayDisconnectedIcon = FAppStyle::GetBrush("Graph.ArrayPin.Disconnected");

		ensure(PinImage.GetNumLayers() == 2);

		struct SGraphPinHack : SGraphPin
		{
			using SGraphPin::GetPinIcon;
		};
		const SGraphPinHack* PinForIcon = static_cast<SGraphPinHack*>(GraphPin.Get());
		const FVoxelPinType PinType(Pin->PinType);

		const bool bIsPromotable = GetVoxelBaseNode().ShowAsPromotableWildcard(*Pin);
		const bool bIsBuffer = PinType.IsBuffer();
		const bool bIsBufferArray = PinType.IsBufferArray();
		const bool bIsPointSet = PinType.Is<FVoxelPointSet>();
		const bool bIsSurface = PinType.Is<FVoxelSurface>();

		PinImage.SetLayerBrush(0, MakeAttributeLambda([=]() -> const FSlateBrush*
		{
			if (!ensure(Pin))
			{
				return nullptr;
			}

			if (bIsBuffer)
			{
				if (bIsBufferArray)
				{
					if (PinForIcon->IsConnected())
					{
						return ArrayConnectedIcon;
					}
					else
					{
						return ArrayDisconnectedIcon;
					}
				}

				if (bIsPromotable)
				{
					return BufferPromotableOuterIcon;
				}

				if (PinForIcon->IsConnected())
				{
					return BufferConnectedIcon;
				}
				else
				{
					return BufferDisconnectedIcon;
				}
			}

			if (bIsPointSet)
			{
				if (PinForIcon->IsConnected())
				{
					return PointSetConnectedIcon;
				}
				else
				{
					return PointSetDisconnectedIcon;
				}
			}

			if (bIsSurface)
			{
				if (PinForIcon->IsConnected())
				{
					return SurfaceConnectedIcon;
				}
				else
				{
					return SurfaceDisconnectedIcon;
				}
			}

			if (bIsPromotable)
			{
				return PromotableOuterIcon;
			}

			return PinForIcon->GetPinIcon();
		}));

		PinImage.SetLayerBrush(1, MakeAttributeLambda([=]() -> const FSlateBrush*
		{
			if (!ensure(Pin))
			{
				return nullptr;
			}

			if (bIsBuffer)
			{
				return BufferPromotableInnerIcon;
			}

			return PromotableInnerIcon;
		}));

		PinImage.SetLayerColor(1, MakeAttributeLambda([=]() -> FSlateColor
		{
			if (!ensure(Pin))
			{
				return FLinearColor::Transparent;
			}

			if (bIsPromotable)
			{
				// Set the inner image to be wildcard color, which is grey by default
				return GetDefault<UGraphEditorSettings>()->WildcardPinTypeColor;
			}

			return FLinearColor::Transparent;
		}));
	};

	INLINE_LAMBDA
	{
		if (!GetVoxelBaseNode().ShouldHideConnectorPin(*Pin))
		{
			return;
		}

		const TSharedPtr<SHorizontalBox> HorizontalRow = GraphPin->GetFullPinHorizontalRowWidget().Pin();
		if (!ensure(HorizontalRow) ||
			!ensure(HorizontalRow->GetChildren()->NumSlot() > 0))
		{
			return;
		}

		SHorizontalBox::FSlot& ConnectorSlot = HorizontalRow->GetSlot(0);
		ConnectorSlot.GetWidget()->SetVisibility(EVisibility::Collapsed);
		ConnectorSlot.SetPadding(0.f);

		const TSharedPtr<SWrapBox> LabelWidget = GraphPin->GetLabelAndValue();
		if (!ensure(LabelWidget) ||
			!ensure(LabelWidget->GetChildren()->NumSlot() > 0))
		{
			return;
		}

		const FSlotBase& LabelSlot = LabelWidget->GetChildren()->GetSlotAt(0);
		LabelSlot.GetWidget()->SetVisibility(EVisibility::Collapsed);
	};

	GraphPin->SetToolTip(
		SNew(SVoxelToolTip)
		.Text(FText::FromString(Pin->PinToolTip))
		.PinType(FVoxelPinType(Pin->PinType)));

	return GraphPin;
}