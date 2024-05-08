// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "Curves/CurveFloat.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelPinValueInterface.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelCurveNode.generated.h"

struct FVoxelCurveData;
struct FVoxelCurveDataWrapper;

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelCurve : public FVoxelPinValueInterface
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Voxel")
	FRichCurve Curve;

	TSharedPtr<FVoxelCurveDataWrapper> Wrapper;

	//~ Begin FVoxelPinValueInterface Interface
	virtual void ComputeRuntimeData() override;
	//~ End FVoxelPinValueInterface Interface

	FORCEINLINE friend uint32 GetTypeHash(const FVoxelCurve VoxelCurve)
	{
		return PointerHash(&VoxelCurve.Curve);
	}
};

struct FVoxelCurveDataWrapper
{
	const TSharedRef<FVoxelDependency> Dependency;

	explicit FVoxelCurveDataWrapper(const TSharedRef<FVoxelDependency>& Dependency)
		: Dependency(Dependency)
	{
	}

	void Update(const FRichCurve& Curve);

	FVoxelFastCriticalSection CriticalSection;
	TSharedPtr<const FVoxelCurveData> Data_RequiresLock;
};

USTRUCT(DisplayName = "Curve")
struct VOXELGRAPHNODES_API FVoxelCurveRef
{
	GENERATED_BODY()

	TWeakObjectPtr<UCurveFloat> Object;
	FVoxelCurve Curve;
};

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelCurveRef);

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelCurveRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelCurveRef, UCurveFloat);
};

UCLASS()
class UVoxelCurveFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Math|Curve")
	FVoxelCurve MakeCurveFromAsset(const FVoxelCurveRef& Asset) const;

	UFUNCTION(Category = "Math|Curve")
	FVoxelFloatBuffer SampleCurve(
		const FVoxelFloatBuffer& Value,
		const FVoxelCurve& Curve,
		UPARAM(meta = (AdvancedDisplay)) bool bFastCurve = true) const;
};