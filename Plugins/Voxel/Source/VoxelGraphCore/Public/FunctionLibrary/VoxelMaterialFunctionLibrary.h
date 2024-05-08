// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFunctionLibrary.h"
#include "Material/VoxelMaterial.h"
#include "VoxelMaterialFunctionLibrary.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialScalarParameter : public FVoxelMaterialParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	FName Name;
	float Value = 0.f;

	virtual TVoxelFutureValue<FVoxelComputedMaterialParameter> Compute(const FVoxelQuery& Query) const override;
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialVectorParameter : public FVoxelMaterialParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	FName Name;
	FVector4 Value = FVector4::Zero();

	virtual TVoxelFutureValue<FVoxelComputedMaterialParameter> Compute(const FVoxelQuery& Query) const override;
};

UCLASS()
class VOXELGRAPHCORE_API UVoxelMaterialFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Material")
	FVoxelMaterialScalarParameter MakeScalarParameter(
		FName Name = "MyValue",
		float Value = 0.f) const;

	UFUNCTION(Category = "Material")
	FVoxelMaterialVectorParameter MakeVectorParameter(
		FName Name = "MyValue",
		const FVector4& Value = FVector4::Zero()) const;
};