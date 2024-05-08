// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelObjectPinType.h"
#include "VoxelMaterial.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelComputedMaterialParameter
{
	GENERATED_BODY()

	TVoxelMap<FName, float> ScalarParameters;
	TVoxelMap<FName, FVector> VectorParameters;
	TVoxelMap<FName, TWeakObjectPtr<UTexture>> TextureParameters;
	TVoxelMap<FName, TSharedPtr<FVoxelDynamicMaterialParameter>> DynamicParameters;
	TVoxelArray<TSharedPtr<FVirtualDestructor>> Resources;

	template<typename LambdaType>
	void ForeachKey(LambdaType&& Lambda) const
	{
		for (const auto& It : ScalarParameters)
		{
			Lambda(It.Key);
		}
		for (const auto& It : VectorParameters)
		{
			Lambda(It.Key);
		}
		for (const auto& It : TextureParameters)
		{
			Lambda(It.Key);
		}
		for (const auto& It : DynamicParameters)
		{
			Lambda(It.Key);
		}
	}
	void Append(const FVoxelComputedMaterialParameter& Other);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialParameter
	: public FVoxelVirtualStruct
	, public FVoxelNodeHelpers
	, public IVoxelNodeInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	FVoxelGraphNodeRef Node;

	//~ Begin IVoxelNodeInterface Interface
	virtual const FVoxelGraphNodeRef& GetNodeRef() const override
	{
		return Node;
	}
	//~ End IVoxelNodeInterface Interface

public:
	VOXEL_SETUP_ON_COMPLETE_MANUAL(FVoxelComputedMaterialParameter, "FVoxelComputedMaterialParameter");

	virtual TVoxelFutureValue<FVoxelComputedMaterialParameter> Compute(const FVoxelQuery& Query) const VOXEL_PURE_VIRTUAL({});
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelComputedMaterial
{
	GENERATED_BODY()

	TSharedPtr<FVoxelMaterialRef> ParentMaterial;
	FVoxelComputedMaterialParameter Parameters;

	TSharedRef<FVoxelMaterialRef> MakeMaterial_GameThread() const;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(DisplayName = "Material")
struct VOXELGRAPHCORE_API FVoxelMaterial
{
	GENERATED_BODY()

	TSharedPtr<FVoxelMaterialRef> ParentMaterial;
	TVoxelArray<TSharedRef<const FVoxelMaterialParameter>> Parameters;

	TVoxelFutureValue<FVoxelComputedMaterial> Compute(const FVoxelQuery& Query) const;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelMaterial);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelMaterial, UMaterialInterface);
};