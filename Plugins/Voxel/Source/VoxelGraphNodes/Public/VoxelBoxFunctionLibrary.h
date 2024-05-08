// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelBoxFunctionLibrary.generated.h"

UCLASS()
class VOXELGRAPHNODES_API UVoxelBoxFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Math|Box", meta = (NativeMakeFunc))
	FVoxelBox MakeBox(
		const FVector& Min,
		const FVector& Max) const
	{
		if (Max.X < Min.X ||
			Max.Y < Min.Y ||
			Max.Z < Min.Z)
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid bounds {1}, {2}", this, Min.ToString(), Max.ToString());
			return {};
		}

		return FVoxelBox(Min, Max);
	}
	UFUNCTION(Category = "Math|Box", meta = (NativeBreakFunc))
	void BreakBox(
		const FVoxelBox& Box,
		FVector& Min,
		FVector& Max) const
	{
		Min = Box.Min;
		Max = Box.Max;
	}

	UFUNCTION(Category = "Math|Box")
	FVoxelBox MakeBoxFromRadius(
		const float Radius) const
	{
		if (Radius <= 0.f)
		{
			VOXEL_MESSAGE(Error, "{0}: Invalid radius {1}", this, Radius);
			return {};
		}

		return FVoxelBox(FVector(-Radius), FVector(Radius));
	}

	UFUNCTION(Category = "Math|Box")
	FVoxelBox MakeInfiniteBox() const
	{
		return FVoxelBox::Infinite;
	}

	UFUNCTION(Category = "Math|Box")
	UPARAM(DisplayName = "World Box") FVoxelBox TransformBox(
		const FVoxelBox& LocalBox,
		const FTransform& LocalToWorld) const
	{
		return LocalBox.TransformBy(LocalToWorld);
	}

	UFUNCTION(Category = "Math|Box")
	FVoxelBox ExtendBox(
		const FVoxelBox& Box,
		const float Amount) const
	{
		const FVector AmountVector = FVoxelUtilities::ComponentMax(-Box.Size() / 2, FVector(Amount));
		return Box.Extend(AmountVector);
	}
};