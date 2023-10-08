// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelArray.h"
#include "VoxelMinimal/Containers/VoxelPackedArray.h"

template<typename T>
class TVoxelPaletteArray
{
public:
	TVoxelPaletteArray() = default;

	template<typename LambdaType>
	void Initialize(int32 Num, LambdaType GetValue)
	{
		Palette.Reset();
		Indices.Reset();
		ArrayNum = Num;

		if (Num == 0)
		{
			return;
		}

		for (int32 Index = 0; Index < Num; Index++)
		{
			Palette.AddUnique(GetValue(Index));
		}
		Palette.Shrink();
		checkVoxelSlow(Palette.Num() >= 1);

		if (Palette.Num() == 1)
		{
			// No need for indices
			return;
		}

		Indices.Initialize(FMath::CeilLogTwo(Palette.Num()), Num);
		for (int32 Index = 0; Index < Num; Index++)
		{
			const int32 PaletteIndex = Palette.Find(GetValue(Index));
			checkVoxelSlow(PaletteIndex != -1);

			Indices[Index] = PaletteIndex;
		}
	}
	template<typename ArrayType>
	void InitializeFrom(const ArrayType& Array)
	{
		static_assert(std::is_same_v<T, typename TDecay<decltype(*GetData(Array))>::Type>, "");

		Initialize(GetNum(Array), [&](int32 Index)
		{
			return GetData(Array)[Index];
		});
	}
	void InitializeUnique(int32 Num, T Value)
	{
		Initialize(Num, [&](int32 Index)
		{
			return Value;
		});
	}

	FORCEINLINE int32 Num() const
	{
		return ArrayNum;
	}
	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return 0 <= Index && Index <= Num();
	}
	FORCEINLINE const T& Get(int32 Index) const
	{
		checkVoxelSlow(IsValidIndex(Index));
		if (Palette.Num() == 1)
		{
			checkVoxelSlow(Indices.Num() == 0);
			return Palette[0];
		}
		else
		{
			checkVoxelSlow(Indices.Num() == ArrayNum);
			return Palette[Indices[Index]];
		}
	}
	FORCEINLINE const T& operator[](int32 Index) const
	{
		return Get(Index);
	}

	FORCEINLINE int64 GetAllocatedSize() const
	{
		return Palette.GetAllocatedSize() + Indices.GetAllocatedSize();
	}

public:
	friend FArchive& operator<<(FArchive& Ar, TVoxelPaletteArray& Array)
	{
		Ar << Array.ArrayNum;
		Ar << Array.Palette;

		if (Array.Palette.Num() > 1)
		{
			Ar << Array.Indices;
		}
		else
		{
			Array.Indices.Reset();
		}

		return Ar;
	}

private:
	int32 ArrayNum = 0;
	TVoxelArray<T> Palette;
	FVoxelPackedArray Indices;
};