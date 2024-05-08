// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelParameterProvider.h"
#include "VoxelGraphInterface.generated.h"

class UVoxelGraph;

UCLASS(Abstract, meta = (AssetColor = LightBlue))
class VOXELGRAPHCORE_API UVoxelGraphInterface
	: public UObject
	, public IVoxelParameterProvider
{
	GENERATED_BODY()

public:
	virtual UVoxelGraph* GetGraph() const VOXEL_PURE_VIRTUAL({});
	virtual FString GetGraphName() const;

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UTexture2D> ThumbnailTexture;
#endif

	friend class UVoxelGraphInterfaceThumbnailRenderer;
};