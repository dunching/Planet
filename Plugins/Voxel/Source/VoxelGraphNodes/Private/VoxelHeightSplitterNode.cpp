// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelHeightSplitterNode.h"
#include "VoxelHeightSplitterNodeImpl.ispc.generated.h"

FVoxelNode_HeightSplitter::FVoxelNode_HeightSplitter()
{
	FixupLayerPins();
}

FVoxelComputeValue FVoxelNode_HeightSplitter::CompileCompute(FName PinName) const
{
	// TODO QueryCache

	int32 Layer = -1;
	for (int32 Index = 0; Index < ResultPins.Num(); Index++)
	{
		if (FName(ResultPins[Index]) == PinName)
		{
			Layer = Index;
			break;
		}
	}
	if (Layer == -1)
	{
		VOXEL_MESSAGE(Error, "{0}: invalid node", this);
		return nullptr;
	}

	return [this, Layer](const FVoxelQuery& Query) -> FVoxelFutureValue
	{
		TVoxelPinRef<FVoxelFloatBuffer> TargetPin = ResultPins[Layer];
		VOXEL_SETUP_ON_COMPLETE(TargetPin)

		const TValue<FVoxelFloatBuffer> HeightBuffer = GetNodeRuntime().Get(HeightPin, Query);

		TVoxelArray<TValue<float>> HeightSplits;
		TVoxelArray<TValue<float>> FalloffSplits;
		HeightSplits.Reserve(LayerPins.Num());
		FalloffSplits.Reserve(LayerPins.Num());
		for (const FLayerPin& LayerPinData : LayerPins)
		{
			HeightSplits.Add(GetNodeRuntime().Get(LayerPinData.Height, Query));
			FalloffSplits.Add(GetNodeRuntime().Get(LayerPinData.Falloff, Query));
		}

		return VOXEL_ON_COMPLETE(HeightBuffer, HeightSplits, FalloffSplits, Layer)
		{
			for (int32 Index = 1; Index < LayerPins.Num(); Index++)
			{
				const float PreviousHeight = HeightSplits[Index - 1];
				const float CurrentHeight = HeightSplits[Index];
				if (PreviousHeight >= CurrentHeight)
				{
					VOXEL_MESSAGE(Error, "{0}: invalid heights at layer {1} and {2}, heights must be in ascending order",
						this,
						Index - 1,
						Index);
					return {};
				}
			}

			FVoxelFloatBufferStorage ReturnValue;
			ReturnValue.Allocate(HeightBuffer.Num());

			// Middle layers
			if (Layer > 0 &&
				Layer < NumLayerPins)
			{
				const float PreviousHeight = HeightSplits[Layer - 1];
				const float PreviousFalloff = FalloffSplits[Layer - 1];

				const float CurrentHeight = HeightSplits[Layer];
				const float CurrentFalloff = FalloffSplits[Layer];

				ForeachVoxelBufferChunk(HeightBuffer.Num(), [&](const FVoxelBufferIterator& Iterator)
				{
					ispc::VoxelNode_HeightSplitter_MiddleLayer(
						HeightBuffer.GetData(Iterator),
						Iterator.Num(),
						PreviousHeight,
						PreviousFalloff,
						CurrentHeight,
						CurrentFalloff,
						ReturnValue.GetData(Iterator));
				});
			}
			// Last layer
			else if (Layer > 0)
			{
				const float PreviousHeight = HeightSplits[Layer - 1];
				const float PreviousFalloff = FalloffSplits[Layer - 1];

				ForeachVoxelBufferChunk(HeightBuffer.Num(), [&](const FVoxelBufferIterator& Iterator)
				{
					ispc::VoxelNode_HeightSplitter_LastLayer(
						HeightBuffer.GetData(Iterator),
						Iterator.Num(),
						PreviousHeight,
						PreviousFalloff,
						ReturnValue.GetData(Iterator));
				});
			}
			// First layer
			else
			{
				const float CurrentHeight = HeightSplits[Layer];
				const float CurrentFalloff = FalloffSplits[Layer];

				ForeachVoxelBufferChunk(HeightBuffer.Num(), [&](const FVoxelBufferIterator& Iterator)
				{
					ispc::VoxelNode_HeightSplitter_FirstLayer(
						HeightBuffer.GetData(Iterator),
						Iterator.Num(),
						CurrentHeight,
						CurrentFalloff,
						ReturnValue.GetData(Iterator));
				});
			}

			return FVoxelFloatBuffer::Make(ReturnValue);
		};
	};
}

void FVoxelNode_HeightSplitter::PostSerialize()
{
	Super::PostSerialize();

	FixupLayerPins();
}

void FVoxelNode_HeightSplitter::FixupLayerPins()
{
	for (const TVoxelPinRef<FVoxelFloatBuffer>& ResultPin : ResultPins)
	{
		RemovePin(ResultPin);
	}
	for (const FLayerPin& Layer : LayerPins)
	{
		RemovePin(Layer.Height);
		RemovePin(Layer.Falloff);
	}

	LayerPins.Reset();
	ResultPins.Reset();

	ResultPins.Add(
		CreateOutputPin<FVoxelFloatBuffer>(
		"Strength_0",
		VOXEL_PIN_METADATA(
			FVoxelFloatBuffer,
			nullptr,
			DisplayName("Strength 1"),
			Tooltip("Strength of this layer")))
	);

	for (int32 Index = 0; Index < NumLayerPins; Index++)
	{
		const FString Category = "Layer " + FString::FromInt(Index);

		LayerPins.Add({
			CreateInputPin<float>(
				FName("Height", Index + 1),
				VOXEL_PIN_METADATA(
					float,
					Index * 1000.f,
					DisplayName("Height"),
					Tooltip("Height of this layer"),
					Category(Category))),

			CreateInputPin<float>(
				FName("Falloff", Index + 1),
				VOXEL_PIN_METADATA(
					float,
					100.f,
					DisplayName("Falloff"),
					Tooltip("Falloff of this layer"),
					Category(Category)))
		});

		ResultPins.Add(CreateOutputPin<FVoxelFloatBuffer>(
			FName("Strength", Index + 2),
			VOXEL_PIN_METADATA(
				FVoxelFloatBuffer,
				nullptr,
				DisplayName("Strength " + LexToString(Index + 2)),
				Tooltip("Strength of this layer"))));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FString FVoxelNode_HeightSplitter::FDefinition::GetAddPinLabel() const
{
	return "Add Layer";
}

FString FVoxelNode_HeightSplitter::FDefinition::GetAddPinTooltip() const
{
	return "Adds an additional layer pin";
}

void FVoxelNode_HeightSplitter::FDefinition::AddInputPin()
{
	Node.NumLayerPins++;
	Node.FixupLayerPins();
}

bool FVoxelNode_HeightSplitter::FDefinition::CanRemoveInputPin() const
{
	return Node.NumLayerPins > 1;
}

void FVoxelNode_HeightSplitter::FDefinition::RemoveInputPin()
{
	Node.NumLayerPins--;
	Node.FixupLayerPins();
}
#endif