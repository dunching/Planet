// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

namespace Voxel::Magica
{

struct FIntTransform
{
	// Matrix elements are accessed with M[RowIndex][ColumnIndex].
	int32 M[4][4];

    FIntTransform()
    {
		M[0][0] = 1; M[0][1] = 0; M[0][2] = 0; M[0][3] = 0;
		M[1][0] = 0; M[1][1] = 1; M[1][2] = 0; M[1][3] = 0;
		M[2][0] = 0; M[2][1] = 0; M[2][2] = 1; M[2][3] = 0;
		M[3][0] = 0; M[3][1] = 0; M[3][2] = 0; M[3][3] = 1;
    }

    static FIntTransform Identity()
    {
		return FIntTransform();
    }

    FORCEINLINE static FIntTransform Multiply(const FIntTransform& TransformA, const FIntTransform& TransformB)
    {
		FIntTransform Result;

    	auto& A = TransformA.M;
        auto& B = TransformB.M;
        auto& R = Result.M;

		R[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
		R[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
		R[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
		R[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

		R[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
		R[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
		R[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
		R[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

		R[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
		R[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
		R[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
		R[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

		R[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
		R[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
		R[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
		R[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];

		return Result;
    }

    FORCEINLINE static FIntTransform Lerp(const FIntTransform& TransformA, const FIntTransform& TransformB, double Alpha)
    {
		FIntTransform Result;
		for (int32 X = 0; X < 4; X++)
		{
			for (int32 Y = 0; Y < 4; Y++)
			{
				Result.M[X][Y] = FMath::RoundToInt(FMath::Lerp<double>(TransformA.M[X][Y], TransformB.M[X][Y], Alpha));
			}
		}

		return Result;
    }

	template<typename VectorType>
	FORCEINLINE static VectorType Multiply(const FIntTransform& Transform, const VectorType& Position)
	{
		return
		{
			Transform.M[0][0] * Position.X + Transform.M[0][1] * Position.Y + Transform.M[0][2] * Position.Z + Transform.M[0][3],
			Transform.M[1][0] * Position.X + Transform.M[1][1] * Position.Y + Transform.M[1][2] * Position.Z + Transform.M[1][3],
			Transform.M[2][0] * Position.X + Transform.M[2][1] * Position.Y + Transform.M[2][2] * Position.Z + Transform.M[2][3]
		};
	}
};

struct FRGBA
{
	uint8 R = 0;
	uint8 G = 0;
	uint8 B = 0;
	uint8 A = 0;
};
static_assert(sizeof(FRGBA) == sizeof(uint32), "");

struct FVoxel
{
    uint8 X = 0;
    uint8 Y = 0;
    uint8 Z = 0;
    uint8 Value = 0;
};
static_assert(sizeof(FVoxel) == sizeof(uint32), "");

using FPalette = TVoxelStaticArray<FRGBA, 256>;

template<typename T>
struct TFrameData
{
	struct FFrame
	{
		T Data;
		int32 FrameIndex = 0;
	};
	TVoxelArray<FFrame> Frames;

	void SortFrames()
	{
		Frames.Sort([](const FFrame& A, const FFrame& B)
		{
			ensureVoxelSlow(A.FrameIndex != B.FrameIndex);
			return A.FrameIndex < B.FrameIndex;
		});
	}
	T Get(int32 FrameIndex) const
	{
		if (!ensure(Frames.Num() > 0))
		{
			return {};
		}

		for (int32 Index = 1; Index < Frames.Num(); Index++)
		{
			if (FrameIndex < Frames[Index].FrameIndex)
			{
				return Frames[Index - 1].Data;
			}
		}

		return Frames.Last().Data;
	}
};

struct FIntTransformFrameData : TFrameData<FIntTransform>
{
	FIntTransform Get(int32 FrameIndex) const
	{
		if (!ensure(Frames.Num() > 0))
		{
			return {};
		}

		for (int32 Index = 1; Index < Frames.Num(); Index++)
		{
			if (FrameIndex < Frames[Index].FrameIndex)
			{
				const double Alpha = (Frames[Index - 1].FrameIndex - FrameIndex) / double(Frames[Index - 1].FrameIndex - Frames[Index].FrameIndex);
				return FIntTransform::Lerp(Frames[Index - 1].Data, Frames[Index].Data, Alpha);
			}
		}

		return Frames.Last().Data;
	}
};

struct FModel
{
	FIntVector Size{ ForceInit };
    TVoxelArray<FVoxel> Voxels;
};

struct FLayer
{
	FString Name;
    bool bHidden = false;
};

struct FGroup
{
	FString Name;
    bool bHidden = false;
	FIntTransformFrameData Transform;

	TSharedPtr<FLayer> Layer;
    TSharedPtr<FGroup> ParentGroup;
};

struct FInstance
{
	FString Name;
    bool bHidden = false;

	FIntTransformFrameData Transform;
	TFrameData<TSharedPtr<FModel>> Model;

	TSharedPtr<FLayer> Layer;
    TSharedPtr<FGroup> ParentGroup;
};

struct FMagicaScene
{
	int32 NumFrames = 0;
	FPalette Palette{ NoInit };
	TVoxelArray<TSharedPtr<FModel>> Models;
	TVoxelArray<TSharedPtr<FLayer>> Layers;
	TVoxelArray<TSharedPtr<FGroup>> Groups;
	TVoxelArray<TSharedPtr<FInstance>> Instances;
};

VOXELCORE_API bool ReadScene(const TVoxelArrayView<const uint8>& Data, FMagicaScene& OutScene);

}