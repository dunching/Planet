// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelChannel.h"
#include "VoxelDeveloperSettings.h"
#include "VoxelSettings.generated.h"

class UVoxelFloatDetailTexture;

UCLASS(config = Engine, DefaultConfig, meta = (DisplayName = "Voxel Plugin"))
class VOXELGRAPHCORE_API UVoxelSettings : public UVoxelDeveloperSettings
{
	GENERATED_BODY()

public:
	UVoxelSettings()
	{
		SectionName = "Voxel Plugin";
	}

public:
	// Number of threads allocated for the voxel background processing. Setting it too high may impact performance
	// The threads are shared across all voxel worlds
	// Can be set using voxel.NumThreads
	UPROPERTY(Config, EditAnywhere, Category = "Config", meta = (ClampMin = 1, ConsoleVariable = "voxel.NumThreads"))
	int32 NumberOfThreads = 0;

	UPROPERTY(Config, EditAnywhere, Category = "Config")
	TArray<FVoxelChannelExposedDefinition> GlobalChannels;

public:
	// If true graphs will have their own thumbnails rendered
	UPROPERTY(Config, EditAnywhere, Category = "Editor")
	bool bEnableGraphThumbnails = true;

public:
	UPROPERTY(Config, EditAnywhere, Category = "Debug")
	TSoftObjectPtr<UMaterialInterface> MarchingCubeDebugMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftClassPath("/Voxel/Debug/MarchingCubeDebugMaterial.MarchingCubeDebugMaterial"));

	UPROPERTY(Config, EditAnywhere, Category = "Debug")
	TArray<TSoftObjectPtr<UVoxelFloatDetailTexture>> MarchingCubeDebugDetailTextures =
	{
		TSoftObjectPtr<UVoxelFloatDetailTexture>(FSoftClassPath("/Voxel/Debug/MarchingCubeDebugDetailTextureR.MarchingCubeDebugDetailTextureR")),
		TSoftObjectPtr<UVoxelFloatDetailTexture>(FSoftClassPath("/Voxel/Debug/MarchingCubeDebugDetailTextureG.MarchingCubeDebugDetailTextureG")),
		TSoftObjectPtr<UVoxelFloatDetailTexture>(FSoftClassPath("/Voxel/Debug/MarchingCubeDebugDetailTextureB.MarchingCubeDebugDetailTextureB"))
	};

public:
	UPROPERTY(Config, EditAnywhere, Category = "Safety", meta = (ClampMin = 1))
	bool bEnablePerformanceMonitoring = true;

	// Number of frames to collect the average frame rate from
	UPROPERTY(Config, EditAnywhere, Category = "Safety", meta = (ClampMin = 2))
	int32 FramesToAverage = 128;

	// Minimum average FPS allowed in specified number of frames
	UPROPERTY(Config, EditAnywhere, Category = "Safety", meta = (ClampMin = 1))
	int32 MinFPS = 8;

	void UpdateChannels();

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
};