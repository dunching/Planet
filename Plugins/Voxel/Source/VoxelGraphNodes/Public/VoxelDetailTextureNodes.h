// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelDetailTexture.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Material/VoxelMaterial.h"
#include "Material/VoxelMaterialDefinitionInterface.h"
#include "VoxelDetailTextureNodes.generated.h"

struct FVoxelMarchingCubeSurface;

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelDetailTextureQueryHelperImpl
{
	GENERATED_BODY()

public:
	void SetupQueryParameters(FVoxelQueryParameters& Parameters) const;

private:
	int32 TextureSize = 0;
	FVoxelVectorBuffer QueryPositions;
	TSharedPtr<const FVoxelMarchingCubeSurface> Surface;

	friend class FVoxelDetailTextureQueryHelper;
};

class VOXELGRAPHNODES_API FVoxelDetailTextureQueryHelper
{
public:
	const TSharedRef<const FVoxelMarchingCubeSurface> Surface;

	explicit FVoxelDetailTextureQueryHelper(const TSharedRef<const FVoxelMarchingCubeSurface>& Surface)
		: Surface(Surface)
	{
	}

	int32 NumCells() const;
	TVoxelFutureValue<FVoxelDetailTextureQueryHelperImpl> GetImpl(int32 TextureSize);

	int32 AddCellCoordinates(TVoxelArray<FVoxelDetailTextureCoordinate>&& CellCoordinates);
	TVoxelArray<uint8> BuildCellIndexToDirection() const;
	TVoxelArray<FVoxelDetailTextureCoordinate> BuildCellCoordinates() const;

private:
	mutable FVoxelFastCriticalSection CriticalSection;
	TVoxelMap<int32, TVoxelFutureValue<FVoxelDetailTextureQueryHelperImpl>> TextureSizeToImpl;
	TVoxelArray<TVoxelArray<FVoxelDetailTextureCoordinate>> AllCellCoordinates;

	FVoxelVectorBuffer ComputeQueryPositions(int32 TextureSize) const;
};

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelDetailTextureQueryParameter : public FVoxelQueryParameter
{
	GENERATED_BODY()
	GENERATED_VOXEL_QUERY_PARAMETER_BODY()

	TSharedPtr<FVoxelDetailTextureQueryHelper> Helper;
};

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelDetailTextureParameter : public FVoxelMaterialParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	TOptional<FName> NameOverride;
	TSharedPtr<FVoxelDetailTexturePool> Pool;
	TSharedPtr<const TVoxelComputeValue<FVoxelBuffer>> GetBuffer;

	template<typename T>
	void SetGetBuffer(const TSharedRef<const TVoxelComputeValue<T>>& GetBufferTyped)
	{
		GetBuffer = ReinterpretCastSharedPtr<const TVoxelComputeValue<FVoxelBuffer>>(GetBufferTyped);
	}

	virtual TVoxelFutureValue<FVoxelComputedMaterialParameter> Compute(const FVoxelQuery& Query) const final override;

protected:
	virtual void ComputeCell(
		int32 Index,
		int32 Pitch,
		int32 TextureSize,
		EPixelFormat PixelFormat,
		int32 TextureIndex,
		const FVoxelBuffer& Buffer,
		TVoxelArrayView<uint8> Data) const VOXEL_PURE_VIRTUAL();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelNormalDetailTextureParameter : public FVoxelDetailTextureParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	virtual void ComputeCell(
		int32 Index,
		int32 Pitch,
		int32 TextureSize,
		EPixelFormat PixelFormat,
		int32 TextureIndex,
		const FVoxelBuffer& Buffer,
		TVoxelArrayView<uint8> Data) const override;
};

USTRUCT(Category = "Material")
struct VOXELGRAPHNODES_API FVoxelNode_MakeNormalDetailTextureParameter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_CALL_PARAM(bool, bIsMain);

	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Normal, nullptr);
	VOXEL_INPUT_PIN(FVoxelNormalDetailTextureRef, Texture, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelMaterialParameter, Parameter);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelFloatDetailTextureParameter : public FVoxelDetailTextureParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	virtual void ComputeCell(
		int32 Index,
		int32 Pitch,
		int32 TextureSize,
		EPixelFormat PixelFormat,
		int32 TextureIndex,
		const FVoxelBuffer& Buffer,
		TVoxelArrayView<uint8> Data) const override;
};

USTRUCT(Category = "Material", meta = (Keywords = "scalar"))
struct VOXELGRAPHNODES_API FVoxelNode_MakeFloatDetailTextureParameter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Float, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatDetailTextureRef, Texture, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelMaterialParameter, Parameter);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelColorDetailTextureParameter : public FVoxelDetailTextureParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	virtual void ComputeCell(
		int32 Index,
		int32 Pitch,
		int32 TextureSize,
		EPixelFormat PixelFormat,
		int32 TextureIndex,
		const FVoxelBuffer& Buffer,
		TVoxelArrayView<uint8> Data) const override;
};

USTRUCT(Category = "Material")
struct VOXELGRAPHNODES_API FVoxelNode_MakeColorDetailTextureParameter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelLinearColorBuffer, Color, nullptr);
	VOXEL_INPUT_PIN(FVoxelColorDetailTextureRef, Texture, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelMaterialParameter, Parameter);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelMaterialIdDetailTextureParameter : public FVoxelDetailTextureParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	virtual void ComputeCell(
		int32 Index,
		int32 Pitch,
		int32 TextureSize,
		EPixelFormat PixelFormat,
		int32 TextureIndex,
		const FVoxelBuffer& Buffer,
		TVoxelArrayView<uint8> Data) const override;
};

USTRUCT(Category = "Material")
struct VOXELGRAPHNODES_API FVoxelNode_MakeMaterialIdDetailTextureParameter : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_CALL_PARAM(bool, bIsMain);

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr);
	VOXEL_INPUT_PIN(FVoxelMaterialIdDetailTextureRef, Texture, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelMaterialParameter, Parameter);
};