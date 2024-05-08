// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGpuTexture.h"
#include "TextureResource.h"

class FVoxelGpuTextureResource : public FTextureResource
{
public:
	UVoxelGpuTexture& Owner;
	FIntVector CachedSize = FIntVector::ZeroValue;

	explicit FVoxelGpuTextureResource(UVoxelGpuTexture& Owner)
		: Owner(Owner)
	{
	}

	virtual void InitRHI(UE_503_ONLY(FRHICommandListBase& RHICmdList)) override
	{
		FSamplerStateInitializerRHI SamplerStateInitializer;
		SamplerStateInitializer.Filter = SF_Bilinear;
		SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);
	}

	virtual void ReleaseRHI() override
	{
		FTextureResource::ReleaseRHI();
		RHIUpdateTextureReference(Owner.TextureReference.TextureReferenceRHI, nullptr);
	}

	virtual uint32 GetSizeX() const override { return CachedSize.X; }
	virtual uint32 GetSizeY() const override { return CachedSize.Y; }
	virtual uint32 GetSizeZ() const override { return CachedSize.Z; }
};

void UVoxelGpuTexture::Update_RenderThread(const TRefCountPtr<IPooledRenderTarget>& NewPooledRenderTarget)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInRenderingThread());

	PooledRenderTarget = NewPooledRenderTarget;

	FVoxelGpuTextureResource* TypedResource = static_cast<FVoxelGpuTextureResource*>(GetResource());
	if (!ensure(TypedResource))
	{
		return;
	}

	FRHITexture* NewTexture = PooledRenderTarget ? PooledRenderTarget->GetRHI() : nullptr;

	TypedResource->CachedSize = NewTexture ? NewTexture->GetSizeXYZ() : FIntVector::ZeroValue;

	if (TypedResource->TextureRHI != NewTexture)
	{
		TypedResource->TextureRHI = NewTexture;
		RHIUpdateTextureReference(TextureReference.TextureReferenceRHI, NewTexture);
	}
}

FTextureResource* UVoxelGpuTexture::CreateResource()
{
	return new FVoxelGpuTextureResource(*this);
}