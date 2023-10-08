// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBounds.h"
#include "VoxelTransformRef.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Material/VoxelMaterialDefinition.h"
#include "VoxelSurface.generated.h"

class FVoxelDetailTexturePool;

namespace ispc
{
	struct FVoxelSurfaceLayer
	{
		bool bConstantMaterials = false;
		const uint16* Materials = nullptr;
		bool bConstantStrengths = false;
		const uint8* Strengths = nullptr;
	};
}
#define __ISPC_STRUCT_FVoxelSurfaceLayer__

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSurfaceMaterial : public FVoxelBufferInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	struct FLayer
	{
		FVoxelMaterialDefinitionBuffer Material;
		FVoxelByteBuffer Strength;
	};
	TVoxelArray<FLayer> Layers;

	int32 Num() const;
	virtual int32 Num_Slow() const final override;
	virtual bool IsValid_Slow() const final override;

	void GetLayers(
		const FVoxelBufferIterator& Iterator,
		TVoxelArray<ispc::FVoxelSurfaceLayer>& OutLayers) const;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelSurface
{
	GENERATED_BODY()

public:
	bool bIsValid = false;
	FVoxelGraphNodeRef Node;
	FVoxelBounds Bounds;

	TSharedPtr<const TVoxelComputeValue<FVoxelFloatBuffer>> ComputeDistance;
	TSharedPtr<const TVoxelComputeValue<FVoxelSurfaceMaterial>> ComputeMaterial;

	struct FAttribute
	{
		FVoxelPinType InnerType;
		TWeakPtr<FVoxelDetailTexturePool> DetailTexturePool;
		TSharedPtr<const TVoxelComputeValue<FVoxelBuffer>> Compute;
	};
	TVoxelMap<FName, FAttribute> NameToAttribute;

public:
	FVoxelSurface() = default;

	static FVoxelSurface Make(
		const FVoxelGraphNodeRef& Node,
		const FVoxelBounds& Bounds);

	static FVoxelSurface MakeInfinite(const FVoxelGraphNodeRef& Node);

	static FVoxelSurface MakeWithLocalBounds(
		const FVoxelGraphNodeRef& Node,
		const FVoxelQuery& Query,
		const FVoxelBox& Bounds);

public:
	void CopyMaterialAttributes(const FVoxelSurface& Other);

	void LerpMaterialAttributes(
		const FVoxelQuery& InQuery,
		const FVoxelGraphNodeRef& NodeRef,
		const FVoxelSurface& A,
		const FVoxelSurface& B,
		const TSharedRef<const TVoxelComputeValue<FVoxelFloatBuffer>>& ComputeAlpha);

public:
	TVoxelFutureValue<FVoxelFloatBuffer> GetDistance(const FVoxelQuery& Query) const;
	TVoxelFutureValue<FVoxelSurfaceMaterial> GetMaterial(const FVoxelQuery& Query) const;

public:
	void SetDistance(
		const FVoxelQuery& Query,
		const FVoxelGraphNodeRef& NodeRef,
		TVoxelComputeValue<FVoxelFloatBuffer>&& Lambda);

	void SetMaterial(
		const FVoxelQuery& Query,
		const FVoxelGraphNodeRef& NodeRef,
		TVoxelComputeValue<FVoxelSurfaceMaterial>&& Lambda);

	void SetAttribute(
		FName Name,
		const FVoxelPinType& InnerType,
		const TWeakPtr<FVoxelDetailTexturePool>& DetailTexturePool,
		const FVoxelQuery& Query,
		const FVoxelGraphNodeRef& NodeRef,
		TVoxelComputeValue<FVoxelBuffer>&& Lambda);

public:
	void SetLocalDistance(
		const FVoxelQuery& Query,
		const FVoxelGraphNodeRef& NodeRef,
		TVoxelComputeValue<FVoxelFloatBuffer>&& Lambda);

	void SetLocalMaterial(
		const FVoxelQuery& Query,
		const FVoxelGraphNodeRef& NodeRef,
		TVoxelComputeValue<FVoxelSurfaceMaterial>&& Lambda);

	void SetLocalAttribute(
		FName Name,
		const FVoxelPinType& InnerType,
		const TWeakPtr<FVoxelDetailTexturePool>& DetailTexturePool,
		const FVoxelQuery& Query,
		const FVoxelGraphNodeRef& NodeRef,
		TVoxelComputeValue<FVoxelBuffer>&& Lambda);
};