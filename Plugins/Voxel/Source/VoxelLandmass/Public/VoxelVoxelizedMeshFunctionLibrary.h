// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "Engine/StaticMesh.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelVoxelizedMeshData.h"
#include "VoxelVoxelizedMeshAsset.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelVoxelizedMeshFunctionLibrary.generated.h"

USTRUCT()
struct VOXELLANDMASS_API FVoxelVoxelizedMesh
{
	GENERATED_BODY()

	TWeakObjectPtr<UVoxelVoxelizedMeshAsset> Asset;
	TSharedPtr<const FVoxelVoxelizedMeshData> Data;
};

USTRUCT()
struct VOXELLANDMASS_API FVoxelVoxelizedMeshStaticMesh
{
	GENERATED_BODY()

	TWeakObjectPtr<UStaticMesh> StaticMesh;
	FVoxelVoxelizedMesh VoxelizedMesh;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelVoxelizedMesh);

USTRUCT()
struct VOXELLANDMASS_API FVoxelVoxelizedMeshPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelVoxelizedMesh, UVoxelVoxelizedMeshAsset)
	{
		if (bSetObject)
		{
			Object = Struct.Asset;
		}
		else
		{
			Struct.Asset = Object;
			Struct.Data = Object->GetMeshData();
		}
	}
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelVoxelizedMeshStaticMesh);

USTRUCT()
struct VOXELLANDMASS_API FVoxelVoxelizedMeshStaticMeshPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelVoxelizedMeshStaticMesh, UStaticMesh)
	{
		if (bSetObject)
		{
			Object = Struct.StaticMesh;
		}
		else
		{
			Struct.StaticMesh = Object;

			if (const UVoxelVoxelizedMeshAsset* Asset = UVoxelVoxelizedMeshAssetUserData::GetAsset(*Object))
			{
				Struct.VoxelizedMesh.Data = Asset->GetMeshData();
			}
		}
	}
};

UCLASS()
class VOXELLANDMASS_API UVoxelVoxelizedMeshFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Voxelized Mesh")
	FVoxelVoxelizedMesh MakeVoxelizedMeshFromStaticMesh(const FVoxelVoxelizedMeshStaticMesh& StaticMesh) const;

	// @param	bHermiteInterpolation	Enabling hermite interpolation can lead to higher quality results (less artifacts when interpolating), but is up to 4x more expensive
	UFUNCTION(Category = "Voxelized Mesh")
	FVoxelSurface CreateVoxelizedMeshSurface(
		const FVoxelVoxelizedMesh& Mesh,
		bool bHermiteInterpolation = false) const;

	FVoxelFloatBuffer CreateVoxelizedMeshSurface_Distance(
		const TSharedRef<const FVoxelVoxelizedMeshData>& MeshData,
		bool bHermiteInterpolation) const;
};