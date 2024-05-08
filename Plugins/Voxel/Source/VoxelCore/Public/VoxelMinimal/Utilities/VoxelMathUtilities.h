// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"
#include "VoxelMinimal/Containers/VoxelStaticArray.h"
#include "VoxelMinimal/Utilities/VoxelBaseUtilities.h"
#include "VoxelMinimal/Utilities/VoxelVectorUtilities.h"
#include "VoxelMinimal/Utilities/VoxelIntVectorUtilities.h"
#include "Algo/IsSorted.h"
#include "VoxelMathUtilities.generated.h"

UENUM(BlueprintType)
enum class EVoxelFalloff : uint8
{
	Linear,
	Smooth,
	Spherical,
	Tip
};

template<typename T>
struct TVoxelWelfordVariance
{
	T Average = FVoxelUtilities::MakeSafe<T>();
	T ScaledVariance = FVoxelUtilities::MakeSafe<T>();
	int32 Num = 0;

	FORCEINLINE void Add(const T& Value)
	{
		Num++;

		const T Delta = Value - Average;
		Average += Delta / Num;
		ScaledVariance += Delta * (Value - Average);
	}

	FORCEINLINE T GetVariance() const
	{
		ensureVoxelSlow(Num > 0);
		if (Num <= 1)
		{
			return FVoxelUtilities::MakeSafe<T>();
		}
		else
		{
			return ScaledVariance / (Num - 1);
		}
	}
};

namespace FVoxelUtilities
{
	template<
		typename ArrayTypeA,
		typename ArrayTypeB,
		typename OutArrayTypeA,
		typename OutArrayTypeB,
		typename PredicateType>
		void DiffSortedArrays(
		const ArrayTypeA& ArrayA,
		const ArrayTypeB& ArrayB,
		OutArrayTypeA& OutOnlyInA,
		OutArrayTypeB& OutOnlyInB,
		PredicateType Less)
	{
		VOXEL_FUNCTION_COUNTER();

		checkVoxelSlow(Algo::IsSorted(ArrayA, Less));
		checkVoxelSlow(Algo::IsSorted(ArrayB, Less));

		int64 IndexA = 0;
		int64 IndexB = 0;
		while (IndexA < ArrayA.Num() && IndexB < ArrayB.Num())
		{
			const auto& A = ArrayA[IndexA];
			const auto& B = ArrayB[IndexB];

			if (Less(A, B))
			{
				OutOnlyInA.Add(A);
				IndexA++;
			}
			else if (Less(B, A))
			{
				OutOnlyInB.Add(B);
				IndexB++;
			}
			else
			{
				IndexA++;
				IndexB++;
			}
		}

		while (IndexA < ArrayA.Num())
		{
			OutOnlyInA.Add(ArrayA[IndexA]);
			IndexA++;
		}

		while (IndexB < ArrayB.Num())
		{
			OutOnlyInB.Add(ArrayB[IndexB]);
			IndexB++;
		}

		checkVoxelSlow(IndexA == ArrayA.Num());
		checkVoxelSlow(IndexB == ArrayB.Num());
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	template<typename T>
	FORCEINLINE T Get2DIndex(const int32 SizeX, const int32 SizeY, const int32 X, const int32 Y)
	{
		checkVoxelSlow(0 <= X && X < SizeX);
		checkVoxelSlow(0 <= Y && Y < SizeY);
		checkVoxelSlow(int64(SizeX) * int64(SizeY) <= TNumericLimits<T>::Max());
		return T(X) + T(Y) * SizeX;
	}
	template<typename T>
	FORCEINLINE T Get2DIndex(const int32 Size, const int32 X, const int32 Y)
	{
		return Get2DIndex<T>(Size, Size, X, Y);
	}
	template<typename T>
	FORCEINLINE T Get2DIndex(const FIntPoint& Size, const int32 X, const int32 Y)
	{
		return Get2DIndex<T>(Size.X, Size.Y, X, Y);
	}
	template<typename T>
	FORCEINLINE T Get2DIndex(const int32 Size, const FIntPoint& Position)
	{
		return Get2DIndex<T>(Size, Position.X, Position.Y);
	}
	template<typename T>
	FORCEINLINE T Get2DIndex(const FIntPoint& Size, const FIntPoint& Position)
	{
		return Get2DIndex<T>(Size, Position.X, Position.Y);
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	template<typename T>
	FORCEINLINE T Get3DIndex(const FIntVector& Size, const int32 X, const int32 Y, const int32 Z)
	{
		checkVoxelSlow(0 <= X && X < Size.X);
		checkVoxelSlow(0 <= Y && Y < Size.Y);
		checkVoxelSlow(0 <= Z && Z < Size.Z);
		checkVoxelSlow(int64(Size.X) * int64(Size.Y) * int64(Size.Z) <= TNumericLimits<T>::Max());
		return T(X) + T(Y) * Size.X + T(Z) * Size.X * Size.Y;
	}
	template<typename T>
	FORCEINLINE T Get3DIndex(const int32 Size, const int32 X, const int32 Y, const int32 Z)
	{
		return Get3DIndex<T>(FIntVector(Size), X, Y, Z);
	}
	template<typename T>
	FORCEINLINE T Get3DIndex(const FIntVector& Size, const FIntVector& Position)
	{
		return Get3DIndex<T>(Size, Position.X, Position.Y, Position.Z);
	}
	template<typename T>
	FORCEINLINE T Get3DIndex(const int32 Size, const FIntVector& Position)
	{
		return Get3DIndex<T>(FIntVector(Size), Position);
	}

	template<typename T>
	FORCEINLINE FIntVector Break3DIndex_Log2(const int32 SizeLog2, const T Index)
	{
		const int32 X = (Index >> (0 * SizeLog2)) & ((1 << SizeLog2) - 1);
		const int32 Y = (Index >> (1 * SizeLog2)) & ((1 << SizeLog2) - 1);
		const int32 Z = (Index >> (2 * SizeLog2)) & ((1 << SizeLog2) - 1);
		checkVoxelSlow(Get3DIndex<T>(1 << SizeLog2, X, Y, Z) == Index);
		return { X, Y, Z };
	}

	template<typename T>
	FORCEINLINE FIntVector Break3DIndex(const FIntVector& Size, T Index)
	{
		const T OriginalIndex = Index;

		const int32 Z = Index / (Size.X * Size.Y);
		checkVoxelSlow(0 <= Z && Z < Size.Z);
		Index -= Z * Size.X * Size.Y;

		const int32 Y = Index / Size.X;
		checkVoxelSlow(0 <= Y && Y < Size.Y);
		Index -= Y * Size.X;

		const int32 X = Index;
		checkVoxelSlow(0 <= X && X < Size.X);

		checkVoxelSlow(Get3DIndex<T>(Size, X, Y, Z) == OriginalIndex);

		return { X, Y, Z };
	}
	template<typename T>
	FORCEINLINE FIntVector Break3DIndex(const int32 Size, const T Index)
	{
		return FVoxelUtilities::Break3DIndex<T>(FIntVector(Size), Index);
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	FORCEINLINE float LinearFalloff(float Distance, float Radius, float Falloff)
	{
		return Distance <= Radius
			? 1.0f
			: Radius + Falloff <= Distance
			? 0.f
			: 1.0f - (Distance - Radius) / Falloff;
	}
	FORCEINLINE float SmoothFalloff(float Distance, float Radius, float Falloff)
	{
		const float X = LinearFalloff(Distance, Radius, Falloff);
		return FMath::SmoothStep(0.f, 1.f, X);
	}
	FORCEINLINE float SphericalFalloff(float Distance, float Radius, float Falloff)
	{
		return Distance <= Radius
			? 1.0f
			: Radius + Falloff <= Distance
			? 0.f
			: FMath::Sqrt(1.0f - FMath::Square((Distance - Radius) / Falloff));
	}
	FORCEINLINE float TipFalloff(float Distance, float Radius, float Falloff)
	{
		return Distance <= Radius
			? 1.0f
			: Radius + Falloff <= Distance
			? 0.f
			: 1.0f - FMath::Sqrt(1.0f - FMath::Square((Falloff + Radius - Distance) / Falloff));
	}

	// Falloff: between 0 and 1
	FORCEINLINE float GetFalloff(EVoxelFalloff FalloffType, float Distance, float Radius, float Falloff)
	{
		Falloff = FMath::Clamp(Falloff, 0.f, 1.f);
		if (Falloff == 0.f)
		{
			return Distance <= Radius ? 1.f : 0.f;
		}

		const float RelativeRadius = Radius * (1.f - Falloff);
		const float RelativeFalloff = Radius * Falloff;
		switch (FalloffType)
		{
		default: VOXEL_ASSUME(false);
		case EVoxelFalloff::Linear:
		{
			return LinearFalloff(Distance, RelativeRadius, RelativeFalloff);
		}
		case EVoxelFalloff::Smooth:
		{
			return SmoothFalloff(Distance, RelativeRadius, RelativeFalloff);
		}
		case EVoxelFalloff::Spherical:
		{
			return SphericalFalloff(Distance, RelativeRadius, RelativeFalloff);
		}
		case EVoxelFalloff::Tip:
		{
			return TipFalloff(Distance, RelativeRadius, RelativeFalloff);
		}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	FORCEINLINE float SmoothMin(const float DistanceA, const float DistanceB, const float Smoothness)
	{
		const float H = FMath::Clamp(0.5f + 0.5f * (DistanceB - DistanceA) / Smoothness, 0.0f, 1.0f);
		return FMath::Lerp(DistanceB, DistanceA, H) - Smoothness * H * (1.0f - H);
	}
	FORCEINLINE float SmoothMax(const float DistanceA, const float DistanceB, const float Smoothness)
	{
		return -SmoothMin(-DistanceA, -DistanceB, Smoothness);
	}

	// See https://www.iquilezles.org/www/articles/smin/smin.htm
	// Unlike SmoothMin this is order-independent
	FORCEINLINE float ExponentialSmoothMin(float DistanceA, float DistanceB, float Smoothness)
	{
		ensureVoxelSlow(Smoothness > 0);
		const float H = FMath::Exp(-DistanceA / Smoothness) + FMath::Exp(-DistanceB / Smoothness);
		return -FMath::Loge(H) * Smoothness;
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	// H00
	FORCEINLINE float HermiteP0(float T)
	{
		return (1 + 2 * T) * FMath::Square(1 - T);
	}
	// H10
	FORCEINLINE float HermiteD0(float T)
	{
		return T * FMath::Square(1 - T);
	}

	// H01
	FORCEINLINE float HermiteP1(float T)
	{
		return FMath::Square(T) * (3 - 2 * T);
	}
	// H11
	FORCEINLINE float HermiteD1(float T)
	{
		return FMath::Square(T) * (T - 1);
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	template<typename VectorType>
	FORCEINLINE bool SegmentAreIntersecting(
		const VectorType& StartA,
		const VectorType& EndA,
		const VectorType& StartB,
		const VectorType& EndB)
	{
		const VectorType VectorA = EndA - StartA;
		const VectorType VectorB = EndB - StartB;

		const auto S =
			(VectorA.X * (StartA.Y - StartB.Y) - VectorA.Y * (StartA.X - StartB.X)) /
			(VectorA.X * VectorB.Y - VectorA.Y * VectorB.X);

		const auto T =
			(VectorB.X * (StartB.Y - StartA.Y) - VectorB.Y * (StartB.X - StartA.X)) /
			(VectorB.X * VectorA.Y - VectorB.Y * VectorA.X);

		return
			0 <= S && S <= 1 &&
			0 <= T && T <= 1;
	}

	FORCEINLINE bool RayTriangleIntersection(
		const FVector3f& RayOrigin,
		const FVector3f& RayDirection,
		const FVector3f& VertexA,
		const FVector3f& VertexB,
		const FVector3f& VertexC,
		const bool bAllowNegativeTime,
		float& OutTime,
		FVector3f* OutBarycentrics = nullptr)
	{
		const FVector3f Diff = RayOrigin - VertexA;
		const FVector3f Edge1 = VertexB - VertexA;
		const FVector3f Edge2 = VertexC - VertexA;
		const FVector3f Normal = FVector3f::CrossProduct(Edge1, Edge2);

		// With:
		// Q = Diff, D = RayDirection, E1 = Edge1, E2 = Edge2, N = Cross(E1, E2)
		//
		// Solve:
		// Q + t * D = b1 * E1 + b2 * E2
		//
		// Using:
		//   |Dot(D, N)| * b1 = sign(Dot(D, N)) * Dot(D, Cross(Q, E2))
		//   |Dot(D, N)| * b2 = sign(Dot(D, N)) * Dot(D, Cross(E1, Q))
		//   |Dot(D, N)| * t = -sign(Dot(D, N)) * Dot(Q, N)

		float Dot = RayDirection.Dot(Normal);
		float Sign;
		if (Dot > KINDA_SMALL_NUMBER)
		{
			Sign = 1;
		}
		else if (Dot < -KINDA_SMALL_NUMBER)
		{
			Sign = -1;
			Dot = -Dot;
		}
		else
		{
			// Ray and triangle are parallel
			return false;
		}

		const float DotTimesB1 = Sign * RayDirection.Dot(Diff.Cross(Edge2));
		if (DotTimesB1 < 0)
		{
			// b1 < 0, no intersection
			return false;
		}

		const float DotTimesB2 = Sign * RayDirection.Dot(Edge1.Cross(Diff));
		if (DotTimesB2 < 0)
		{
			// b2 < 0, no intersection
			return false;
		}

		if (DotTimesB1 + DotTimesB2 > Dot)
		{
			// b1 + b2 > 1, no intersection
			return false;
		}

		// Line intersects triangle, check if ray does.
		const float DotTimesT = -Sign * Diff.Dot(Normal);
		if (DotTimesT < 0 && !bAllowNegativeTime)
		{
			// t < 0, no intersection
			return false;
		}

		// Ray intersects triangle.
		OutTime = DotTimesT / Dot;

		if (OutBarycentrics)
		{
			OutBarycentrics->Y = DotTimesB1 / Dot;
			OutBarycentrics->Z = DotTimesB2 / Dot;
			OutBarycentrics->X = 1 - OutBarycentrics->Y - OutBarycentrics->Z;
		}

		return true;
	}

	FORCEINLINE FVector3f GetTriangleCrossProduct(const FVector3f& A, const FVector3f& B, const FVector3f& C)
	{
		return FVector3f::CrossProduct(C - A, B - A);
	}
	FORCEINLINE FVector3f GetTriangleNormal(const FVector3f& A, const FVector3f& B, const FVector3f& C)
	{
		return GetTriangleCrossProduct(A, B, C).GetSafeNormal();
	}
	FORCEINLINE float GetTriangleArea(const FVector3f& A, const FVector3f& B, const FVector3f& C)
	{
		return GetTriangleCrossProduct(A, B, C).Size() / 2.f;
	}
	FORCEINLINE bool IsTriangleValid(const FVector3f& A, const FVector3f& B, const FVector3f& C, float Tolerance = KINDA_SMALL_NUMBER)
	{
		return GetTriangleArea(A, B, C) > Tolerance;
	}

	FORCEINLINE FVector GetTriangleNormal(const FVector& A, const FVector& B, const FVector& C)
	{
		return FVector::CrossProduct(C - A, B - A).GetSafeNormal();
	}
	FORCEINLINE double GetTriangleArea(const FVector& A, const FVector& B, const FVector& C)
	{
		return FVector::CrossProduct(C - A, B - A).Size() / 2.f;
	}
	FORCEINLINE bool IsTriangleValid(const FVector& A, const FVector& B, const FVector& C, float Tolerance = KINDA_SMALL_NUMBER)
	{
		return GetTriangleArea(A, B, C) > Tolerance;
	}

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	VOXELCORE_API FQuat MakeQuaternionFromEuler(double Pitch, double Yaw, double Roll);
	VOXELCORE_API FQuat MakeQuaternionFromBasis(const FVector& X, const FVector& Y, const FVector& Z);
	VOXELCORE_API FQuat MakeQuaternionFromZ(const FVector& Z);

	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	FORCEINLINE FVector2f UnitVectorToOctahedron(FVector3f Unit)
	{
		ensureVoxelSlow(Unit.IsNormalized());

		const float AbsSum = FMath::Abs(Unit.X) + FMath::Abs(Unit.Y) + FMath::Abs(Unit.Z);
		Unit.X /= AbsSum;
		Unit.Y /= AbsSum;

		FVector2f Result = FVector2f(Unit.X, Unit.Y);
		if (Unit.Z <= 0)
		{
			Result.X = (1 - FMath::Abs(Unit.Y)) * (Unit.X >= 0 ? 1 : -1);
			Result.Y = (1 - FMath::Abs(Unit.X)) * (Unit.Y >= 0 ? 1 : -1);
		}
		return Result * 0.5f + 0.5f;
	}
	FORCEINLINE FVector3f OctahedronToUnitVector(FVector2f Octahedron)
	{
		ensureVoxelSlow(0 <= Octahedron.X && Octahedron.X <= 1);
		ensureVoxelSlow(0 <= Octahedron.Y && Octahedron.Y <= 1);

		Octahedron = Octahedron * 2.f - 1.f;

		FVector3f Unit;
		Unit.X = Octahedron.X;
		Unit.Y = Octahedron.Y;
		Unit.Z = 1.f - FMath::Abs(Octahedron.X) - FMath::Abs(Octahedron.Y);

		const float T = FMath::Max(-Unit.Z, 0.f);

		Unit.X += Unit.X >= 0 ? -T : T;
		Unit.Y += Unit.Y >= 0 ? -T : T;

		ensureVoxelSlow(Unit.SizeSquared() >= KINDA_SMALL_NUMBER);

		return Unit.GetUnsafeNormal();
	}
}

#define __ISPC_STRUCT_FVoxelOctahedron__

namespace ispc
{
	struct FVoxelOctahedron
	{
		uint8 X;
		uint8 Y;
	};
}

struct FVoxelOctahedron : ispc::FVoxelOctahedron
{
	FVoxelOctahedron() = default;
	FORCEINLINE explicit FVoxelOctahedron(EForceInit)
	{
		X = 0;
		Y = 0;
	}
	FORCEINLINE explicit FVoxelOctahedron(const FVector2f& Octahedron)
	{
		X = FVoxelUtilities::FloatToUINT8(Octahedron.X);
		Y = FVoxelUtilities::FloatToUINT8(Octahedron.Y);

		ensureVoxelSlow(0 <= Octahedron.X && Octahedron.X <= 1);
		ensureVoxelSlow(0 <= Octahedron.Y && Octahedron.Y <= 1);
	}
	FORCEINLINE explicit FVoxelOctahedron(const FVector3f& UnitVector)
		: FVoxelOctahedron(FVoxelUtilities::UnitVectorToOctahedron(UnitVector))
	{
	}

	FORCEINLINE FVector2f GetOctahedron() const
	{
		return
		{
				FVoxelUtilities::UINT8ToFloat(X),
				FVoxelUtilities::UINT8ToFloat(Y)
		};
	}
	FORCEINLINE FVector3f GetUnitVector() const
	{
		return FVoxelUtilities::OctahedronToUnitVector(GetOctahedron());
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FVoxelOctahedron& Octahedron)
	{
		return Ar << Octahedron.X << Octahedron.Y;
	}
};