// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "SGraphNodeKnot.h"

class SVoxelGraphNodeKnot : public SGraphNodeKnot
{
public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& Args, UVoxelGraphNode* InNode)
	{
		SGraphNodeKnot::Construct(SGraphNodeKnot::FArguments(), InNode);
	}

	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) const override
	{
		static const FSlateBrush* BufferConnectedIcon = FVoxelEditorStyle::GetBrush("Pin.Buffer.Connected");
		static const FSlateBrush* BufferDisconnectedIcon = FVoxelEditorStyle::GetBrush("Pin.Buffer.Disconnected");

		TSharedPtr<SGraphPin> GraphPin = SGraphNodeKnot::CreatePinWidget(Pin);
		SLayeredImage& PinImage = static_cast<SLayeredImage&>(*GraphPin->GetPinImageWidget());

		ensure(PinImage.GetNumLayers() == 2);

		struct SGraphPinHack : SGraphPin
		{
			using SGraphPin::GetPinIcon;
		};
		const SGraphPinHack* PinForIcon = static_cast<SGraphPinHack*>(GraphPin.Get());

		const bool bIsBuffer = FVoxelPinType(Pin->PinType).IsBuffer();

		PinImage.SetLayerBrush(0, MakeAttributeLambda([=]() -> const FSlateBrush*
		{
			if (!ensure(Pin))
			{
				return nullptr;
			}

			if (bIsBuffer)
			{
				if (PinForIcon->IsConnected())
				{
					return BufferConnectedIcon;
				}
				else
				{
					return BufferDisconnectedIcon;
				}
			}

			return PinForIcon->GetPinIcon();
		}));

		return GraphPin;
	}

	UVoxelGraphNode* GetVoxelNode() const
	{
		return CastChecked<UVoxelGraphNode>(GraphNode);
	}
};