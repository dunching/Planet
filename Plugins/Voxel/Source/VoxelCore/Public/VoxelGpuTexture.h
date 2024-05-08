// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelGpuTexture.generated.h"

UCLASS(Transient)
class VOXELCORE_API UVoxelGpuTexture : public UTexture
{
	GENERATED_BODY()

public:
	void Update_RenderThread(const TRefCountPtr<IPooledRenderTarget>& NewPooledRenderTarget);

	//~ Begin UTexture Interface
	virtual float GetSurfaceWidth() const override { ensure(false); return 0; }
	virtual float GetSurfaceHeight() const override { ensure(false); return 0; }
	virtual FTextureResource* CreateResource() override;
	virtual EMaterialValueType GetMaterialType() const override { return MCT_Texture2D; }
	//~ End UTexture Interface

private:
	TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
};