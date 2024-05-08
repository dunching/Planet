// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelExecNode.h"
#include "VoxelObjectPinType.h"
#include "Engine/VolumeTexture.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelWriteVolumeTextureExecNode.generated.h"

USTRUCT(DisplayName = "Volume Texture")
struct VOXELGRAPHNODES_API FVoxelVolumeTexture
{
	GENERATED_BODY()

	TWeakObjectPtr<UVolumeTexture> Texture;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelVolumeTexture);

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelVolumeTexturePinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelVolumeTexture, UVolumeTexture)
	{
		if (bSetObject)
		{
			Object = Struct.Texture;
		}
		else
		{
			Struct.Texture = Object;
		}
	}
};

USTRUCT(DisplayName = "Write Volume Texture")
struct VOXELGRAPHNODES_API FVoxelWriteVolumeTextureExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelVolumeTexture, Texture, nullptr, ConstantPin);
	VOXEL_INPUT_PIN(FVector, Start, nullptr, ConstantPin);
	VOXEL_INPUT_PIN(FIntVector, Size, FIntVector(128), ConstantPin);
	VOXEL_INPUT_PIN(float, VoxelSize, 100.f, ConstantPin);
	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Distance, nullptr, VirtualPin);

	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELGRAPHNODES_API FVoxelWriteVolumeTextureExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelWriteVolumeTextureExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	//~ End FVoxelExecNodeRuntime Interface

private:
	TVoxelDynamicValue<FVoxelFloatBuffer> DistanceValue;
};