// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelHeightmap.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelHeightmapFunctionLibrary.generated.h"

USTRUCT()
struct VOXELLANDMASS_API FVoxelHeightmapRef
{
	GENERATED_BODY()

	TWeakObjectPtr<UVoxelHeightmap> Asset;
	FVoxelHeightmapConfig Config;
	TSharedPtr<const FVoxelHeightmap> Data;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapRef);

USTRUCT()
struct VOXELLANDMASS_API FVoxelHeightmapRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelHeightmapRef, UVoxelHeightmap)
	{
		if (bSetObject)
		{
			Object = Struct.Asset;
		}
		else
		{
			Struct.Asset = Object;
			Struct.Config = Object->Config;
			Struct.Data = Object->Heightmap;
		}
	}
};

UCLASS()
class VOXELLANDMASS_API UVoxelHeightmapFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	// Will clamp position if outside of the heightmap bounds
	// Heightmap is centered, ie position is between -Size/2 and Size/2
	UFUNCTION(Category = "Heightmap")
	FVoxelFloatBuffer SampleHeightmap(
		const FVoxelHeightmapRef& Heightmap,
		const FVoxelVector2DBuffer& Position) const;

	// Creates a planet from 6 heightmaps
	// All the heightmaps should have the same size
	// All the heightmap settings will be ignored - heightmap values will be between 0 and MaxHeight
	UFUNCTION(Category = "Heightmap")
	FVoxelSurface MakeCubemapPlanetSurface(
		UPARAM(DisplayName = "+X") const FVoxelHeightmapRef& PosX,
		UPARAM(DisplayName = "-X") const FVoxelHeightmapRef& NegX,
		UPARAM(DisplayName = "+Y") const FVoxelHeightmapRef& PosY,
		UPARAM(DisplayName = "-Y") const FVoxelHeightmapRef& NegY,
		UPARAM(DisplayName = "+Z") const FVoxelHeightmapRef& PosZ,
		UPARAM(DisplayName = "-Z") const FVoxelHeightmapRef& NegZ,
		const FVector& PlanetCenter,
		float PlanetRadius = 100000.f,
		float MaxHeight = 10000.f) const;

	FVoxelFloatBuffer MakeCubemapPlanetSurface_Distance(
		const FVoxelHeightmapRef& PosX,
		const FVoxelHeightmapRef& NegX,
		const FVoxelHeightmapRef& PosY,
		const FVoxelHeightmapRef& NegY,
		const FVoxelHeightmapRef& PosZ,
		const FVoxelHeightmapRef& NegZ,
		const FVector& PlanetCenter,
		float PlanetRadius,
		float MaxHeight) const;

	UFUNCTION(Category = "Heightmap")
	FVoxelBox GetHeightmapBounds(const FVoxelHeightmapRef& Heightmap) const;
};