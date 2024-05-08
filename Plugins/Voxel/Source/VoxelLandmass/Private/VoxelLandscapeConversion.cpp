// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelLandscapeConversion.h"
#include "VoxelHeightmap.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "LandscapeDataAccess.h"

bool FVoxelLandscapeConversion::Convert(const ALandscape& Landscape, UVoxelHeightmap& Heightmap)
{
	ensure(false);
#if WITH_EDITOR && 0 // TODO
	VOXEL_FUNCTION_COUNTER();

	const ULandscapeInfo* LandscapeInfo = Landscape.GetLandscapeInfo();
	if (!LandscapeInfo)
	{
		return false;
	}

	int32 Width = 0;
	int32 Height = 0;
	LandscapeInfo->ForAllLandscapeComponents([&](const ULandscapeComponent* Component)
	{
		Width = FMath::Max(Width, Component->SectionBaseX + Component->ComponentSizeQuads);
		Height = FMath::Max(Height, Component->SectionBaseY + Component->ComponentSizeQuads);
	});

	// Account for additional vertex row/column at the end
	Width++;
	Height++;

	const TSharedRef<FVoxelHeightmap> Data = MakeVoxelShared<FVoxelHeightmap>();
	Data->SetSize(Width, Height);

	struct FLayerChannel
	{
		FVoxelHeightmapAssetChannelIndex Channel;
		ULandscapeLayerInfoObject* LayerInfo;
	};
	TArray<FLayerChannel> LayerChannels;
	for (const FLandscapeInfoLayerSettings& Layer : LandscapeInfo->Layers)
	{
		if (!Layer.LayerInfoObj)
		{
			continue;
		}

		const FVoxelHeightmapAssetChannelIndex Channel = Data->RegisterChannel<FVoxelUnitScalar8>(Layer.LayerName);
		LayerChannels.Add({ Channel, Layer.LayerInfoObj });
	}

	TVoxelArray<float> Heights;
	Heights.SetNumZeroed(Width * Height);

	LandscapeInfo->ForAllLandscapeComponents([&](ULandscapeComponent* Component)
	{
		FLandscapeComponentDataInterface DataInterface(Component);

		// + 1 : account for additional vertex row/column at the end of each components
		// This will result in some data being written twice, but that's fine
		const int32 ComponentSize = Component->ComponentSizeQuads + 1;
		const int32 WeightmapSize = (Component->SubsectionSizeQuads + 1) * Component->NumSubsections;

		TArray<TArray<uint8>> WeightmapDatas;
		for (const FLayerChannel& LayerChannel : LayerChannels)
		{
			TArray<uint8>& WeightmapData = WeightmapDatas.Emplace_GetRef();
			DataInterface.GetWeightmapTextureData(LayerChannel.LayerInfo, WeightmapData);
			check(WeightmapData.Num() == 0 || WeightmapData.Num() == WeightmapSize * WeightmapSize);
		}

		for (int32 X = 0; X < ComponentSize; X++)
		{
			for (int32 Y = 0; Y < ComponentSize; Y++)
			{
				const FVector Vertex = DataInterface.GetWorldVertex(X, Y);
				const FVector LocalVertex = (Vertex - Landscape.GetActorLocation()) / Component->GetComponentTransform().GetScale3D();

				const int64 LocalVertexX = int64(LocalVertex.X);
				const int64 LocalVertexY = int64(LocalVertex.Y);
				if (!ensure(Data->IsValidIndex(LocalVertexX, LocalVertexY)))
				{
					continue;
				}

				Heights[FVoxelUtilities::Get2DIndex(Width, Height, LocalVertexX, LocalVertexY)] = Vertex.Z;

				for (int32 LayerIndex = 0; LayerIndex < LayerChannels.Num(); LayerIndex++)
				{
					const FLayerChannel& LayerChannel = LayerChannels[LayerIndex];
					const TArray<uint8>& WeightmapData = WeightmapDatas[LayerIndex];

					FVoxelUnitScalar8 Value;
					if (WeightmapData.Num() > 0)
					{
						Value.SetValue(FVoxelUtilities::Get2D(WeightmapData, WeightmapSize, X, Y));
					}
					Data->Set<FVoxelUnitScalar8>(LayerChannel.Channel, LocalVertexX, LocalVertexY, Value);
				}
			}
		}
	});

	{
		float Min = 0;
		float Max = 1;
		FVoxelHeightmap::NormalizeHeights(Heights, Min, Max);

		const FVoxelHeightmapAssetChannelIndex Channel = Data->RegisterChannel<float>(FVoxelChannelNames::Height);
		Data->GetChannelData(Channel) = FVoxelTypedData(Heights);

		Heightmap.HeightOffset = Min;
		Heightmap.ScaleZ = Max - Min;
	}

	Heightmap.SetHeightmapData(Data);
	Heightmap.ScaleXY = Landscape.GetActorScale3D().X;

	return true;
#else
	return false;
#endif
}