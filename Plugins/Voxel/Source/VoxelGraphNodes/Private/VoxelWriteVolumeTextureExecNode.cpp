// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelWriteVolumeTextureExecNode.h"
#include "VoxelBufferUtilities.h"
#include "VoxelPositionQueryParameter.h"
#include "TextureResource.h"

TVoxelUniquePtr<FVoxelExecNodeRuntime> FVoxelWriteVolumeTextureExecNode::CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const
{
	return MakeVoxelUnique<FVoxelWriteVolumeTextureExecNodeRuntime>(SharedThis);
}

void FVoxelWriteVolumeTextureExecNodeRuntime::Create()
{
	VOXEL_FUNCTION_COUNTER();
	ensure(IsInGameThread());

	const TWeakObjectPtr<UVolumeTexture> WeakTexture = GetConstantPin(Node.TexturePin).Texture;
	const FVector Start = GetConstantPin(Node.StartPin);
	const FIntVector Size = GetConstantPin(Node.SizePin);
	const float VoxelSize = GetConstantPin(Node.VoxelSizePin);

	if (int64(Size.X) *
		int64(Size.Y) *
		int64(Size.Z) > 1024 * 1024 * 1024)
	{
		ensure(false);
		return;
	}
	if (Size.X <= 0 ||
		Size.Y <= 0 ||
		Size.Z <= 0)
	{
		ensure(false);
		return;
	}

	const TSharedRef<FVoxelQueryParameters> Parameters = MakeVoxelShared<FVoxelQueryParameters>();
	Parameters->Add<FVoxelGradientStepQueryParameter>().Step = VoxelSize;
	Parameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(FVector3f(Start), VoxelSize, Size);

	DistanceValue = GetNodeRuntime().MakeDynamicValueFactory(Node.DistancePin).Compute(GetContext(), Parameters);
	DistanceValue.OnChanged_GameThread([=](const FVoxelFloatBuffer& Distance)
	{
		UVolumeTexture* Texture = WeakTexture.Get();
		if (!Texture)
		{
			return;
		}

#if WITH_EDITOR
		const FTextureSource Source;
		Texture->Source = Source;
#endif

		FTexturePlatformData* PlatformData = new FTexturePlatformData();
		PlatformData->SizeX = Size.X;
		PlatformData->SizeY = Size.Y;
		PlatformData->SetNumSlices(Size.Z);
		PlatformData->PixelFormat = PF_R32_FLOAT;

		FTexture2DMipMap* Mip = new FTexture2DMipMap();
		Mip->SizeX = Size.X;
		Mip->SizeY = Size.Y;
		Mip->SizeZ = Size.Z;
		Mip->BulkData.Lock(LOCK_READ_WRITE);
		{
			void* Data = Mip->BulkData.Realloc(sizeof(float) * Size.X * Size.Y * Size.Z);
			const TVoxelArrayView<float> OutData(static_cast<float*>(Data), Size.X * Size.Y * Size.Z);
			Distance.GetStorage().CopyTo(OutData);
		}
		Mip->BulkData.Unlock();
		PlatformData->Mips.Add(Mip);

		if (const FTexturePlatformData* ExistingPlatformData = Texture->GetPlatformData())
		{
			delete ExistingPlatformData;
			Texture->SetPlatformData(nullptr);
		}

		check(!Texture->GetPlatformData());
		Texture->SetPlatformData(PlatformData);
		Texture->UpdateResource();
	});
}

void FVoxelWriteVolumeTextureExecNodeRuntime::Destroy()
{
	DistanceValue = {};
}