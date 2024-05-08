// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"
#include "VoxelObjectPinType.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "VoxelPhysicalMaterial.generated.h"

USTRUCT(DisplayName = "Physical Material")
struct VOXELGRAPHNODES_API FVoxelPhysicalMaterial
{
	GENERATED_BODY()

	TWeakObjectPtr<UPhysicalMaterial> Material;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelPhysicalMaterial);

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelPhysicalMaterialPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelPhysicalMaterial, UPhysicalMaterial)
	{
		if (bSetObject)
		{
			Object = Struct.Material;
		}
		else
		{
			Struct.Material = Object;
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelPhysicalMaterialBuffer, FVoxelPhysicalMaterial);

USTRUCT(DisplayName = "Physical Material Buffer")
struct VOXELGRAPHNODES_API FVoxelPhysicalMaterialBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelPhysicalMaterialBuffer, FVoxelPhysicalMaterial);
};