// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelMaterialDefinitionInterface.h"
#include "VoxelParameterOverrideCollection_DEPRECATED.h"
#include "VoxelMaterialDefinitionInstance.generated.h"

UCLASS(meta = (VoxelAssetType))
class VOXELGRAPHCORE_API UVoxelMaterialDefinitionInstance : public UVoxelMaterialDefinitionInterface
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UVoxelMaterialDefinitionInterface> Parent_DEPRECATED;

	UPROPERTY()
	FVoxelParameterOverrideCollection_DEPRECATED ParameterCollection_DEPRECATED;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Voxel", meta = (ProviderClass = "/Script/VoxelGraphCore.VoxelMaterialDefinitionInterface"))
	TObjectPtr<UVoxelParameterContainer> ParameterContainer;

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void SetParentDefinition(UVoxelMaterialDefinitionInterface* NewParentDefinition);

public:
	UVoxelMaterialDefinitionInstance();

	//~ Begin UVoxelMaterialDefinitionInterface Interface
	virtual UVoxelMaterialDefinition* GetDefinition() const override;
	//~ End UVoxelMaterialDefinitionInterface Interface

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin IVoxelParameterProvider Interface
	virtual IVoxelParameterProvider* GetSourceProvider() override;
	//~ End IVoxelParameterProvider Interface
};