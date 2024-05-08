// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelSculptFunctionLibrary.generated.h"

class AVoxelActor;
class UVoxelGraphInterface;

UCLASS()
class VOXELGRAPHCORE_API UVoxelSculptFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Voxel|Sculpt")
	static void ApplySculpt(
		AVoxelActor* TargetActor,
		AVoxelActor* SculptActor);
};