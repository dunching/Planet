// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphInstance.h"
#include "VoxelParameterContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelBlueprintFunctionLibrary.generated.h"

class AVoxelActor;

UCLASS()
class VOXELBLUEPRINTEDITOR_API UVoxelBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Creates a Voxel Graph Instance asset from specified Voxel Actor,
	// will use its Graph as parent and copy all parameters
	// @param SourceActor Voxel Actor to copy graph and parameters from
	UFUNCTION(BlueprintCallable, Category = "Voxel|Editor", meta = (ExpandEnumAsExecs = ReturnValue))
	static EVoxelSuccess CreateNewVoxelGraphInstanceFromVoxelActor(
		AVoxelActor* SourceActor,
		FString AssetPathAndName,
		UVoxelGraphInstance*& VoxelGraphInstanceAsset);

	// Creates a Voxel Graph Instance asset with SourceGraph as its parent
	// @param SourceGraph Voxel Graph Asset to use as parent
	UFUNCTION(BlueprintCallable, Category = "Voxel|Editor", meta = (ExpandEnumAsExecs = ReturnValue))
	static EVoxelSuccess CreateNewVoxelGraphInstanceFromGraph(
		UVoxelGraphInterface* SourceGraph,
		FString AssetPathAndName,
		UVoxelGraphInstance*& VoxelGraphInstanceAsset);

	// Copies and applies all parameter values from SourceParametersContainer to DestinationGraph
	// @param SourceParametersContainer Parameters Container to copy values from
	// @param DestinationGraph Graph Instance asset to apply parameter values
	UFUNCTION(BlueprintCallable, Category = "Voxel|Editor", meta = (ExpandEnumAsExecs = ReturnValue))
	static EVoxelSuccess ApplyParametersToGraphInstance(
		UVoxelParameterContainer* SourceParametersContainer,
		UVoxelGraphInstance* DestinationGraph);
};