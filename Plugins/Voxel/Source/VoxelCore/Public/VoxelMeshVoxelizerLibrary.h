// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelDistanceFieldUtilities_Old.h"
#include "VoxelMeshVoxelizerLibrary.generated.h"

struct FVoxelMeshVoxelizerInputData
{
	TVoxelArray<FVector3f> Vertices;
	TVoxelArray<int32> Indices;
	TVoxelArray<FVector3f> VertexNormals;
};

USTRUCT(BlueprintType)
struct VOXELCORE_API FVoxelMeshVoxelizerSettings
{
	GENERATED_BODY()

	// Sweep direction to determine the voxel signs. If you have a plane, use Z
	UPROPERTY(Category = "Voxel", BlueprintReadWrite, EditAnywhere)
	EVoxelAxis SweepDirection = EVoxelAxis::X;

	// Will do the sweep the other way around: eg, if SweepDirection = Z, the sweep will be done top to bottom if true
	UPROPERTY(Category = "Voxel", BlueprintReadWrite, EditAnywhere)
	bool bReverseSweep = true;

	// If true, will assume every line of voxels starts outside the mesh, then goes inside, then goes outside it
	// Set to false if you have a shell and not a true volume
	// For example:
	// - sphere: set to true
	// - half sphere with no bottom geometry: set to false
	UPROPERTY(Category = "Voxel", BlueprintReadWrite, EditAnywhere)
	bool bWatertight = true;

	// If true, will hide leaks by having holes instead
	// If false, leaks will be long tubes going through the entire asset
	UPROPERTY(Category = "Voxel", BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
	bool bHideLeaks = true;

	bool operator==(const FVoxelMeshVoxelizerSettings& Other) const
	{
		return
			SweepDirection == Other.SweepDirection &&
			bReverseSweep == Other.bReverseSweep &&
			bWatertight == Other.bWatertight &&
			bHideLeaks == Other.bHideLeaks;
	}
	bool operator!=(const FVoxelMeshVoxelizerSettings& Other) const
	{
		return !(*this == Other);
	}

	friend FArchive& operator<<(FArchive& Ar, FVoxelMeshVoxelizerSettings& Settings)
	{
		Ar << Settings.SweepDirection;
		Ar << Settings.bReverseSweep;
		Ar << Settings.bWatertight;
		Ar << Settings.bHideLeaks;

		return Ar;
	}
};

UCLASS()
class VOXELCORE_API UVoxelMeshVoxelizerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FVoxelMeshVoxelizerInputData CreateMeshDataFromStaticMesh(const UStaticMesh& StaticMesh);

	static bool VoxelizeMesh(
		const FVoxelMeshVoxelizerInputData& Mesh,
		const FTransform& Transform,
		const FVoxelMeshVoxelizerSettings& Settings,
		// Needed if we want a smooth import, in voxels
		float BoxExtension,
		TVoxelArray<float>& OutDistances,
		TVoxelArray<FVector3f>& OutSurfacePositions,
		FIntVector& OutSize,
		FIntVector& OutOffset,
		int32& OutNumLeaks,
		bool bMultiThreaded = true);
};