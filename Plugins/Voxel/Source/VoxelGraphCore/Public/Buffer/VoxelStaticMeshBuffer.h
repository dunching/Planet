// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"
#include "Engine/StaticMesh.h"
#include "VoxelObjectPinType.h"
#include "VoxelStaticMeshBuffer.generated.h"

USTRUCT(DisplayName = "Static Mesh")
struct VOXELGRAPHCORE_API FVoxelStaticMesh
{
	GENERATED_BODY()

	TWeakObjectPtr<UStaticMesh> StaticMesh;

	struct FMeshInfo
	{
		FBox MeshBox = FBox(ForceInit);
		int32 DesiredInstancesPerLeaf = 0;
	};
	FMeshInfo GetMeshInfo() const;

	static FVoxelStaticMesh Make(UStaticMesh* Mesh);

	FORCEINLINE bool operator==(const FVoxelStaticMesh& Other) const
	{
		return MakeObjectKey(StaticMesh) == MakeObjectKey(Other.StaticMesh);
	}
	FORCEINLINE bool operator!=(const FVoxelStaticMesh& Other) const
	{
		return MakeObjectKey(StaticMesh) != MakeObjectKey(Other.StaticMesh);
	}

	FORCEINLINE friend uint32 GetTypeHash(const FVoxelStaticMesh& Mesh)
	{
		return GetTypeHash(MakeObjectKey(Mesh.StaticMesh));
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelStaticMesh);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelStaticMeshPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelStaticMesh, UStaticMesh)
	{
		if (bSetObject)
		{
			Object = Struct.StaticMesh;
		}
		else
		{
			Struct = FVoxelStaticMesh::Make(Object.Get());
		}
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelStaticMeshBuffer, FVoxelStaticMesh);

USTRUCT(DisplayName = "Static Mesh Buffer")
struct VOXELGRAPHCORE_API FVoxelStaticMeshBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelStaticMeshBuffer, FVoxelStaticMesh);
};