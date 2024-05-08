// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelGraphInterface.h"
#include "VoxelSpawner.generated.h"

UCLASS(meta = (AssetColor = LightGreen))
class VOXELSPAWNER_API UVoxelSpawnerCollection : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Config", EditAnywhere)
	TArray<TObjectPtr<UVoxelGraphInterface>> Assets;
};