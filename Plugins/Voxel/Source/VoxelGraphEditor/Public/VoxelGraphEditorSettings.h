// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelDeveloperSettings.h"
#include "VoxelGraphEditorSettings.generated.h"

UCLASS(config=EditorPerProjectUserSettings)
class VOXELGRAPHEDITOR_API UVoxelGraphEditorSettings : public UVoxelDeveloperSettings
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelDeveloperSettings Interface
	virtual FName GetContainerName() const override;
	//~ End UVoxelDeveloperSettings Interface

public:
	UPROPERTY(Config, EditAnywhere, Category = "Graph")
	FLinearColor PointSetPinColor = FLinearColor(0.607717f, 0.224984f, 1.f, 1.f);

	UPROPERTY(Config, EditAnywhere, Category = "Graph")
	FLinearColor SurfacePinColor = FLinearColor(0.007499f, 0.64448f, 0.730461f, 1.f);

	UPROPERTY(Config, EditAnywhere, Category = "Graph")
	FLinearColor SeedPinColor = FLinearColor(0.607717f, 0.224984f, 1.f, 1.f);

	UPROPERTY(Config, EditAnywhere, Category = "Graph")
	FLinearColor ChannelPinColor = FLinearColor(0.607717f, 0.224984f, 1.f, 1.f);
};