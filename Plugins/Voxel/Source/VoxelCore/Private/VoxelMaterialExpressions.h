// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Materials/MaterialExpressionLandscapeLayerBlend.h"
#include "Materials/MaterialExpressionLandscapeLayerSample.h"
#include "Materials/MaterialExpressionLandscapeLayerSwitch.h"
#include "Materials/MaterialExpressionLandscapeLayerWeight.h"
#include "Materials/MaterialExpressionLandscapeVisibilityMask.h"
#include "VoxelMaterialExpressions.generated.h"

// Keep this file to not break existing materials

UCLASS()
class UMaterialExpressionVoxelLandscapeLayerBlend : public UMaterialExpressionLandscapeLayerBlend
{
	GENERATED_BODY()
};

UCLASS()
class UMaterialExpressionVoxelLandscapeLayerSwitch : public UMaterialExpressionLandscapeLayerSwitch
{
	GENERATED_BODY()
};

UCLASS()
class UMaterialExpressionVoxelLandscapeLayerWeight : public UMaterialExpressionLandscapeLayerWeight
{
	GENERATED_BODY()
};

UCLASS()
class UMaterialExpressionVoxelLandscapeLayerSample : public UMaterialExpressionLandscapeLayerSample
{
	GENERATED_BODY()
};

UCLASS()
class UMaterialExpressionVoxelLandscapeVisibilityMask : public UMaterialExpressionLandscapeVisibilityMask
{
	GENERATED_BODY()
};