// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelHeightmap.generated.h"

DECLARE_VOXEL_MEMORY_STAT(VOXELLANDMASS_API, STAT_VoxelHeightmapMemory, "Voxel Heightmap Memory");

class VOXELLANDMASS_API FVoxelHeightmap
{
public:
	FVoxelHeightmap() = default;

	int64 GetAllocatedSize() const
	{
		return Heights.GetAllocatedSize();
	}
	void Serialize(FArchive& Ar);

	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelHeightmapMemory);

public:
	FORCEINLINE FIntPoint GetSize() const
	{
		return FIntPoint(SizeX, SizeY);
	}
	FORCEINLINE int32 GetSizeX() const
	{
		return SizeX;
	}
	FORCEINLINE int32 GetSizeY() const
	{
		return SizeY;
	}

	FORCEINLINE uint16 GetMinHeight() const
	{
		return MinHeight;
	}
	FORCEINLINE uint16 GetMaxHeight() const
	{
		return MaxHeight;
	}

	FORCEINLINE bool IsValidIndex(int32 X, int32 Y) const
	{
		return
			0 <= X && X < SizeX &&
			0 <= Y && Y < SizeY;
	}
	FORCEINLINE int32 GetIndex(int32 X, int32 Y) const
	{
		checkVoxelSlow(IsValidIndex(X, Y));
		return X + SizeX * Y;
	}

	FORCEINLINE const TVoxelArray<uint16>& GetHeights() const
	{
		return Heights;
	}
	FORCEINLINE float GetHeight(int32 X, int32 Y) const
	{
		return GetHeight(FVoxelUtilities::Get2DIndex<int32>(SizeX, SizeY, X, Y));
	}
	FORCEINLINE float GetHeight(int32 Index) const
	{
		return Heights[Index] / float(MAX_uint16);
	}

public:
	void Initialize(
		int32 NewSizeX,
		int32 NewSizeY,
		TVoxelArray<uint16>&& NewHeights);

private:
	int32 SizeX = 0;
	int32 SizeY = 0;
	uint16 MinHeight = 0;
	uint16 MaxHeight = 0;
	TVoxelArray<uint16> Heights;
};

USTRUCT()
struct VOXELLANDMASS_API FVoxelHeightmapConfig
{
	GENERATED_BODY()

	// In unreal units (cm)
	UPROPERTY(EditAnywhere, Category = "Config")
	float ScaleZ = 1;

	// In unreal units (cm)
	UPROPERTY(EditAnywhere, Category = "Config", meta = (DisplayName = "Scale XY"))
	float ScaleXY = 1;

	UPROPERTY(EditAnywhere, Category = "Config", meta = (UIMin = 0, UIMax = 1))
	float Cutoff = 0;

	UPROPERTY(EditAnywhere, Category = "Config")
	float CutoffSlope = 1;

	UPROPERTY()
	float InternalScaleZ = 0;

	UPROPERTY()
	float InternalOffsetZ = 0;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bEnableCutoff = false;
};

UCLASS(HideDropdown, BlueprintType, meta = (VoxelAssetType, AssetColor=Red))
class VOXELLANDMASS_API UVoxelHeightmap : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Import")
	int32 ImportWidth;

	UPROPERTY(VisibleAnywhere, Category = "Import")
	int32 ImportHeight;

	UPROPERTY(EditAnywhere, Category = "Import", meta = (FilePathFilter = "Heightmap file (*.png; *.raw; *.r16)|*.png;*.raw;*.r16"))
	FFilePath ImportPath;

public:
	UPROPERTY(EditAnywhere, Category = "Config", meta = (ShowOnlyInnerProperties))
	FVoxelHeightmapConfig Config;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bCompress = true;

public:
	TSharedPtr<FVoxelHeightmap> Heightmap;
	TSharedPtr<FVoxelHeightmap> SourceHeightmap;

	FSimpleMulticastDelegate OnPropertyChanged;
	FSimpleMulticastDelegate OnHeightmapUpdated;

	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	void UpdateHeightmap();

private:
	FByteBulkData BulkData;
	FByteBulkData SourceBulkData;
};