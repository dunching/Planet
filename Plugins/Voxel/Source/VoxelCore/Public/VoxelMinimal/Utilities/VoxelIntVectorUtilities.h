// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelMap.h"
#include "VoxelMinimal/Containers/VoxelStaticArray.h"
#include "VoxelMinimal/Utilities/VoxelBaseUtilities.h"

namespace FVoxelUtilities
{
	FORCEINLINE bool CountIs32Bits(const FIntVector& Size)
	{
		return FMath::Abs(int64(Size.X) * int64(Size.Y) * int64(Size.Z)) < MAX_int32;
	}

	FORCEINLINE FIntVector PositiveMod(const FIntVector& V, int32 Divisor)
	{
		return FIntVector(
			PositiveMod(V.X, Divisor),
			PositiveMod(V.Y, Divisor),
			PositiveMod(V.Z, Divisor));
	}

	FORCEINLINE FIntVector DivideFloor(const FIntVector& V, int32 Divisor)
	{
		return FIntVector(
			DivideFloor(V.X, Divisor),
			DivideFloor(V.Y, Divisor),
			DivideFloor(V.Z, Divisor));
	}
	FORCEINLINE FIntVector DivideFloor_Positive(const FIntVector& V, int32 Divisor)
	{
		return FIntVector(
			DivideFloor_Positive(V.X, Divisor),
			DivideFloor_Positive(V.Y, Divisor),
			DivideFloor_Positive(V.Z, Divisor));
	}
	FORCEINLINE FIntVector DivideFloor_FastLog2(const FIntVector& V, int32 DivisorLog2)
	{
		return FIntVector(
			DivideFloor_FastLog2(V.X, DivisorLog2),
			DivideFloor_FastLog2(V.Y, DivisorLog2),
			DivideFloor_FastLog2(V.Z, DivisorLog2));
	}
	FORCEINLINE FIntVector DivideCeil(const FIntVector& V, int32 Divisor)
	{
		return FIntVector(
			DivideCeil(V.X, Divisor),
			DivideCeil(V.Y, Divisor),
			DivideCeil(V.Z, Divisor));
	}
	FORCEINLINE FIntVector DivideRound(const FIntVector& V, int32 Divisor)
	{
		return FIntVector(
			DivideRound(V.X, Divisor),
			DivideRound(V.Y, Divisor),
			DivideRound(V.Z, Divisor));
	}

	FORCEINLINE FIntPoint DivideFloor(const FIntPoint& V, int32 Divisor)
	{
		return FIntPoint(
			DivideFloor(V.X, Divisor),
			DivideFloor(V.Y, Divisor));
	}
	FORCEINLINE FIntPoint DivideCeil(const FIntPoint& V, int32 Divisor)
	{
		return FIntPoint(
			DivideCeil(V.X, Divisor),
			DivideCeil(V.Y, Divisor));
	}
	FORCEINLINE FIntPoint DivideRound(const FIntPoint& V, int32 Divisor)
	{
		return FIntPoint(
			DivideRound(V.X, Divisor),
			DivideRound(V.Y, Divisor));
	}

	FORCEINLINE int64 SizeSquared(const FIntPoint& V)
	{
		return
			FMath::Square<int64>(V.X) +
			FMath::Square<int64>(V.Y);
	}
	FORCEINLINE double Size(const FIntPoint& V)
	{
		return FMath::Sqrt(double(SizeSquared(V)));
	}

	FORCEINLINE int64 SizeSquared(const FIntVector& V)
	{
		return
			FMath::Square<int64>(V.X) +
			FMath::Square<int64>(V.Y) +
			FMath::Square<int64>(V.Z);
	}
	FORCEINLINE double Size(const FIntVector& V)
	{
		return FMath::Sqrt(double(SizeSquared(V)));
	}

	FORCEINLINE bool IsMultipleOfPowerOfTwo(const FIntVector& Vector, int32 PowerOfTwo)
	{
		return
			IsMultipleOfPowerOfTwo(Vector.X, PowerOfTwo) &&
			IsMultipleOfPowerOfTwo(Vector.Y, PowerOfTwo) &&
			IsMultipleOfPowerOfTwo(Vector.Z, PowerOfTwo);
	}

	FORCEINLINE uint64 FastIntVectorHash_DoubleWord(const FIntVector& Key)
	{
		const uint32 X = Key.X;
		const uint32 Y = Key.Y;
		const uint32 Z = Key.Z;

		uint64 Hash = uint64(X) | (uint64(Y) << 32);
		Hash ^= uint64(Z) << 16;

		return MurmurHash64(Hash);
	}
	FORCEINLINE uint32 FastIntVectorHash(const FIntVector& Key)
	{
		return FastIntVectorHash_DoubleWord(Key);
	}
	FORCEINLINE uint32 FastIntVectorHash_Low(const FIntVector& Key)
	{
		return FastIntVectorHash_DoubleWord(Key);
	}
	FORCEINLINE uint32 FastIntVectorHash_High(const FIntVector& Key)
	{
		return FastIntVectorHash_DoubleWord(Key) >> 32;
	}

	FORCEINLINE uint32 FastIntPointHash(const FIntPoint& Key)
	{
		const uint64 Hash = uint64(Key.X) | (uint64(Key.Y) << 32);
		return MurmurHash64(Hash);
	}

}

template <typename ValueType>
struct TVoxelIntVectorMapKeyFuncs : TDefaultMapKeyFuncs<FIntVector, ValueType, false>
{
	FORCEINLINE static uint32 GetKeyHash(const FIntVector& Key)
	{
		return FVoxelUtilities::FastIntVectorHash(Key);
	}
};
struct FVoxelIntVectorSetKeyFuncs : DefaultKeyFuncs<FIntVector>
{
	FORCEINLINE static uint32 GetKeyHash(const FIntVector& Key)
	{
		return FVoxelUtilities::FastIntVectorHash(Key);
	}
};

template <typename ValueType>
struct TVoxelIntPointMapKeyFuncs : TDefaultMapKeyFuncs<FIntPoint, ValueType, false>
{
	FORCEINLINE static uint32 GetKeyHash(const FIntPoint& Key)
	{
		return FVoxelUtilities::FastIntPointHash(Key);
	}
};

template<typename ValueType, typename SetAllocator = FDefaultSetAllocator>
using TVoxelIntVectorMap = TVoxelMap<FIntVector, ValueType, SetAllocator, TVoxelIntVectorMapKeyFuncs<ValueType>>;

template<typename Allocator = FDefaultSetAllocator>
using TVoxelIntVectorSet = TSet<FIntVector, FVoxelIntVectorSetKeyFuncs, Allocator>;

using FVoxelIntVectorSet = TVoxelIntVectorSet<>;

template<typename ValueType, typename SetAllocator = FDefaultSetAllocator>
using TVoxelIntPointMap = TVoxelMap<FIntPoint, ValueType, SetAllocator, TVoxelIntPointMapKeyFuncs<ValueType>>;

template<>
struct TLess<FIntVector>
{
	FORCEINLINE bool operator()(const FIntVector& A, const FIntVector& B) const
	{
		if (A.X != B.X) return A.X < B.X;
		if (A.Y != B.Y) return A.Y < B.Y;
		return A.Z < B.Z;
	}
};

FORCEINLINE FIntVector operator-(const FIntVector& V)
{
	return FIntVector(-V.X, -V.Y, -V.Z);
}

FORCEINLINE FIntVector operator-(const FIntVector& V, int32 I)
{
	return FIntVector(V.X - I, V.Y - I, V.Z - I);
}
FORCEINLINE FIntVector operator-(const FIntVector& V, uint32 I)
{
	return FIntVector(V.X - I, V.Y - I, V.Z - I);
}
FORCEINLINE FIntVector operator-(int32 I, const FIntVector& V)
{
	return FIntVector(I - V.X, I - V.Y, I - V.Z);
}
FORCEINLINE FIntVector operator-(uint32 I, const FIntVector& V)
{
	return FIntVector(I - V.X, I - V.Y, I - V.Z);
}

FORCEINLINE FIntVector operator+(const FIntVector& V, int32 I)
{
	return FIntVector(V.X + I, V.Y + I, V.Z + I);
}
FORCEINLINE FIntVector operator+(const FIntVector& V, uint32 I)
{
	return FIntVector(V.X + I, V.Y + I, V.Z + I);
}
FORCEINLINE FIntVector operator+(int32 I, const FIntVector& V)
{
	return FIntVector(I + V.X, I + V.Y, I + V.Z);
}
FORCEINLINE FIntVector operator+(uint32 I, const FIntVector& V)
{
	return FIntVector(I + V.X, I + V.Y, I + V.Z);
}

FORCEINLINE FIntVector operator*(int32 I, const FIntVector& V)
{
	return FIntVector(I * V.X, I * V.Y, I * V.Z);
}
FORCEINLINE FIntVector operator*(uint32 I, const FIntVector& V)
{
	return FIntVector(I * V.X, I * V.Y, I * V.Z);
}
FORCEINLINE FIntVector operator*(const FIntVector& V, uint32 I)
{
	return FIntVector(I * V.X, I * V.Y, I * V.Z);
}
FORCEINLINE FIntVector operator*(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(A.X * B.X, A.Y * B.Y, A.Z * B.Z);
}

FORCEINLINE bool operator==(const FIntVector& V, int32 I)
{
	return V.X == I && V.Y == I && V.Z == I;
}

FORCEINLINE FIntPoint operator*(int32 I, const FIntPoint& V)
{
	return FIntPoint(I * V.X, I * V.Y);
}
FORCEINLINE FIntPoint operator*(uint32 I, const FIntPoint& V)
{
	return FIntPoint(I * V.X, I * V.Y);
}

FORCEINLINE FVector3f operator*(FVector3f F, const FIntVector& I)
{
	return FVector3f(F.X * I.X, F.Y * I.Y, F.Z * I.Z);
}
FORCEINLINE FVector3d operator*(FVector3d F, const FIntVector& I)
{
	return FVector3d(F.X * I.X, F.Y * I.Y, F.Z * I.Z);
}

template<typename T>
FIntVector operator-(const FIntVector& V, T A) = delete;
template<typename T>
FIntVector operator-(T A, const FIntVector& V) = delete;

template<typename T>
FIntVector operator+(const FIntVector& V, T A) = delete;
template<typename T>
FIntVector operator+(T A, const FIntVector& V) = delete;

template<typename T>
FIntVector operator*(const FIntVector& V, T A) = delete;
template<typename T>
FIntVector operator*(T A, const FIntVector& V) = delete;

template<typename T>
FIntVector operator%(const FIntVector& V, T A) = delete;
template<typename T>
FIntVector operator/(const FIntVector& V, T A) = delete;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FIntPoint operator*(const FIntPoint&, float) = delete;
FIntPoint operator*(float, const FIntPoint& V) = delete;

FIntPoint operator*(const FIntPoint&, double) = delete;
FIntPoint operator*(double, const FIntPoint& V) = delete;

FORCEINLINE FIntPoint operator%(const FIntPoint& A, const FIntPoint& B)
{
	return FIntPoint(A.X % B.X, A.Y % B.Y);
}
FORCEINLINE FIntPoint operator%(const FIntPoint& V, int32 I)
{
	return V % FIntPoint(I);
}
FORCEINLINE FIntPoint operator%(const FIntPoint& V, uint32 I)
{
	return V % FIntPoint(I);
}