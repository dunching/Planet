// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "Engine/AssetUserData.h"
#include "VoxelMeshVoxelizerLibrary.h"
#include "VoxelVoxelizedMeshAsset.generated.h"

class FVoxelVoxelizedMeshData;

UCLASS(BlueprintType, meta = (VoxelAssetType, AssetColor=Red))
class VOXELLANDMASS_API UVoxelVoxelizedMeshAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Voxel", AssetRegistrySearchable)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, Category = "Voxel", meta = (ClampMin = 0.00001))
	float VoxelSize = 20.f;

	// Relative to the size
	// Bigger = higher memory usage but more accurate when using smooth min
	UPROPERTY(EditAnywhere, Category = "Voxel", meta = (UIMin = 0, UIMax = 1))
	float MaxSmoothness = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Voxel", meta = (ShowOnlyInnerProperties))
	FVoxelMeshVoxelizerSettings VoxelizerSettings;

	TSharedPtr<const FVoxelVoxelizedMeshData> GetMeshData() const;

public:
#if WITH_EDITOR
	static void OnReimport();
#endif

public:
	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "Voxel", AdvancedDisplay)
	mutable FIntVector DataSize;

	UPROPERTY(VisibleAnywhere, Category = "Voxel")
	mutable float MemorySizeInMB = 0.f;
#endif

private:
	mutable TSharedPtr<const FVoxelVoxelizedMeshData> MeshData;

	void TryCreateMeshData() const;
};

UCLASS()
class VOXELLANDMASS_API UVoxelVoxelizedMeshAssetUserData : public UAssetUserData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Voxel")
	TSoftObjectPtr<UVoxelVoxelizedMeshAsset> Asset;

	static UVoxelVoxelizedMeshAsset* GetAsset(UStaticMesh& Mesh);
};