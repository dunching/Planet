// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelPositionFunctionLibrary.generated.h"

UCLASS()
class UVoxelPositionFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Misc", meta = (ShowInShortList))
	UPARAM(meta = (NotTemplate)) FVoxelVector2DBuffer GetPosition2D() const
	{
		FindVoxelQueryParameter_Function(FVoxelPositionQueryParameter, PositionQueryParameter);

		const FVoxelVectorBuffer Positions = PositionQueryParameter->GetPositions();

		FVoxelVector2DBuffer Positions2D;
		Positions2D.X = Positions.X;
		Positions2D.Y = Positions.Y;
		return Positions2D;
	}
	UFUNCTION(Category = "Misc", meta = (ShowInShortList))
	UPARAM(meta = (NotTemplate)) FVoxelVectorBuffer GetPosition3D() const
	{
		FindVoxelQueryParameter_Function(FVoxelPositionQueryParameter, PositionQueryParameter);
		return PositionQueryParameter->GetPositions();
	}
};