// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelGraphInterface.h"
#include "VoxelParameterOverrideCollection_DEPRECATED.h"
#include "VoxelGraphInstance.generated.h"

UCLASS(meta = (VoxelAssetType))
class VOXELGRAPHCORE_API UVoxelGraphInstance : public UVoxelGraphInterface
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UVoxelGraphInterface> Parent_DEPRECATED;

	UPROPERTY()
	FVoxelParameterOverrideCollection_DEPRECATED ParameterCollection_DEPRECATED;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Voxel", meta = (ProviderClass = "/Script/VoxelGraphCore.VoxelGraphInterface"))
	TObjectPtr<UVoxelParameterContainer> ParameterContainer;

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void SetParentGraph(UVoxelGraphInterface* NewParentGraph);

public:
	UVoxelGraphInstance();

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin UVoxelGraphInterface Interface
	virtual UVoxelGraph* GetGraph() const override;
	//~ End UVoxelGraphInterface Interface

	//~ Begin IVoxelParameterProvider Interface
	virtual IVoxelParameterProvider* GetSourceProvider() override;
	//~ End IVoxelParameterProvider Interface
};