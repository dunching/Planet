// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelParameter.h"
#include "VoxelMaterialDefinitionInterface.h"
#include "VoxelMaterialDefinition.generated.h"

struct FVoxelMaterialParameterData;
class UVoxelMaterialDefinitionInstance;

UCLASS(meta = (VoxelAssetType))
class VOXELGRAPHCORE_API UVoxelMaterialDefinition : public UVoxelMaterialDefinitionInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UMaterialInterface> PreviewMaterial;

	UPROPERTY(EditAnywhere, Category = "Internal")
	TArray<FVoxelParameter> Parameters;

	UPROPERTY()
	FVoxelParameterCategories Categories;

	UPROPERTY(EditAnywhere, Category = "Internal", meta = (StructTypeConst))
#if CPP
	TMap<FGuid, TVoxelInstancedStruct<FVoxelMaterialParameterData>> GuidToParameterData;
#else
	TMap<FGuid, FVoxelInstancedStruct> GuidToParameterData;
#endif

	mutable FSimpleMulticastDelegate OnParametersChanged;

public:
	const FVoxelParameter* FindParameterByGuid(const FGuid& ParameterGuid)
	{
		return Parameters.FindByKey(ParameterGuid);
	}

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UVoxelMaterialDefinitionInterface Interface
	virtual UVoxelMaterialDefinition* GetDefinition() const override
	{
		return ConstCast(this);
	}
	//~ End UVoxelMaterialDefinitionInterface Interface

	//~ Begin IVoxelParameterProvider Interface
	virtual void AddOnChanged(const FSimpleDelegate& Delegate) override;
	virtual TSharedPtr<IVoxelParameterRootView> GetParameterViewImpl(const FVoxelParameterPath& BasePath) override;
	//~ End IVoxelParameterProvider Interface

public:
	void QueueRebuildTextures();
	void RebuildTextures();
};