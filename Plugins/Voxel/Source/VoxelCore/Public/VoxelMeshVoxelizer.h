// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelMeshVoxelizerLibrary.h"

namespace Voxel::MeshVoxelizer
{

FORCEINLINE void GetTriangleBarycentrics(
	const FVector3f& P,
	const FVector3f& A,
	const FVector3f& B,
	const FVector3f& C,
	float& OutAlphaA,
	float& OutAlphaB,
	float& OutAlphaC)
{
	const FVector3f CA = A - C;
	const FVector3f CB = B - C;
	const FVector3f CP = P - C;

	const float SizeCA = CA.SizeSquared();
	const float SizeCB = CB.SizeSquared();

	const float a = FVector3f::DotProduct(CA, CP);
	const float b = FVector3f::DotProduct(CB, CP);
	const float d = FVector3f::DotProduct(CA, CB);

	const float Det = SizeCA * SizeCB - d * d;
	checkVoxelSlow(Det > 0.f);

	OutAlphaA = (SizeCB * a - d * b) / Det;
	OutAlphaB = (SizeCA * b - d * a) / Det;
	OutAlphaC = 1 - OutAlphaA - OutAlphaB;

	ensureVoxelSlow(OutAlphaA > 0 || OutAlphaB > 0 || OutAlphaC > 0);
}

FORCEINLINE float PointSegmentDistanceSquared(
	const FVector3f& P,
	const FVector3f& A,
	const FVector3f& B,
	float& OutAlpha)
{
	// DotProduct(PB, AB.GetSafeNormal()) / AB.Size()
	// = DotProduct(PB, AB / AB.Size()) / AB.Size()
	// = DotProduct(PB, AB) / AB.Size() / AB.Size()
	// = DotProduct(PB, AB) / AB.SizeSquared()

	const FVector3f AB = B - A;
	const FVector3f PB = B - P;

	OutAlpha = FVector3f::DotProduct(PB, AB) / AB.SizeSquared();
	OutAlpha = FMath::Clamp(OutAlpha, 0.f, 1.f);

	return FVector3f::DistSquared(P, FMath::Lerp(B, A, OutAlpha));
}
FORCEINLINE float PointSegmentDistanceSquared(
	const FVector3f& P,
	const FVector3f& A,
	const FVector3f& B)
{
	float Alpha;
	return PointSegmentDistanceSquared(P, B, A, Alpha);
}

FORCEINLINE float PointTriangleDistanceSquared(
	const FVector3f& P,
	const FVector3f& A,
	const FVector3f& B,
	const FVector3f& C)
{
	float AlphaA;
	float AlphaB;
	float AlphaC;
	GetTriangleBarycentrics(P, A, B, C, AlphaA, AlphaB, AlphaC);

	if (AlphaA >= 0 && AlphaB >= 0 && AlphaC >= 0)
	{
		// If we're inside the triangle
		return FVector3f::DistSquared(P, AlphaA * A + AlphaB * B + AlphaC * C);
	}

	if (AlphaA > 0)
	{
		// Rules out BC
		return FMath::Min(
			PointSegmentDistanceSquared(P, A, B),
			PointSegmentDistanceSquared(P, A, C));
	}
	else if (AlphaB > 0)
	{
		// Rules out AC
		return FMath::Min(
			PointSegmentDistanceSquared(P, B, C),
			PointSegmentDistanceSquared(P, B, A));
	}
	else
	{
		ensureVoxelSlow(AlphaC > 0);
		// Rules out AB
		return FMath::Min(
			PointSegmentDistanceSquared(P, C, A),
			PointSegmentDistanceSquared(P, C, B));
	}
}

FORCEINLINE float PointTriangleDistanceSquared(
	const FVector3f& P,
	const FVector3f& A,
	const FVector3f& B,
	const FVector3f& C,
	float& OutAlphaA,
	float& OutAlphaB,
	float& OutAlphaC)
{
	GetTriangleBarycentrics(P, A, B, C, OutAlphaA, OutAlphaB, OutAlphaC);

	if (OutAlphaA >= 0 && OutAlphaB >= 0 && OutAlphaC >= 0)
	{
		// If we're inside the triangle
		return FVector3f::Dist(P, OutAlphaA * A + OutAlphaB * B + OutAlphaC * C);
	}

	if (OutAlphaA > 0)
	{
		// Rules out BC
		float AlphaAB;
		const float DistanceAB = PointSegmentDistanceSquared(P, A, B, AlphaAB);
		float AlphaAC;
		const float DistanceAC = PointSegmentDistanceSquared(P, A, C, AlphaAC);

		if (DistanceAB < DistanceAC)
		{
			OutAlphaA = AlphaAB;
			OutAlphaB = 1 - AlphaAB;
			OutAlphaC = 0;
			return DistanceAB;
		}
		else
		{
			OutAlphaA = AlphaAC;
			OutAlphaB = 0;
			OutAlphaC = 1 - AlphaAC;
			return DistanceAC;
		}
	}
	else if (OutAlphaB > 0)
	{
		// Rules out AC
		float AlphaAB;
		const float DistanceAB = PointSegmentDistanceSquared(P, A, B, AlphaAB);
		float AlphaBC;
		const float DistanceBC = PointSegmentDistanceSquared(P, B, C, AlphaBC);

		if (DistanceAB < DistanceBC)
		{
			OutAlphaA = AlphaAB;
			OutAlphaB = 1 - AlphaAB;
			OutAlphaC = 0;
			return DistanceAB;
		}
		else
		{
			OutAlphaA = 0;
			OutAlphaB = AlphaBC;
			OutAlphaC = 1 - AlphaBC;
			return DistanceBC;
		}
	}
	else
	{
		ensureVoxelSlow(OutAlphaC > 0);
		// Rules out AB

		float AlphaBC;
		const float DistanceBC = PointSegmentDistanceSquared(P, B, C, AlphaBC);
		float AlphaAC;
		const float DistanceAC = PointSegmentDistanceSquared(P, A, C, AlphaAC);

		if (DistanceBC < DistanceAC)
		{
			OutAlphaA = 0;
			OutAlphaB = AlphaBC;
			OutAlphaC = 1 - AlphaBC;
			return DistanceBC;
		}
		else
		{
			OutAlphaA = AlphaAC;
			OutAlphaB = 0;
			OutAlphaC = 1 - AlphaAC;
			return DistanceAC;
		}
	}
}

VOXELCORE_API void Voxelize(
	const FVoxelMeshVoxelizerSettings& Settings,
	const TVoxelArray<FVector3f>& Vertices,
	const TVoxelArray<int32>& Indices,
	const FVector3f& Origin,
	const FIntVector& Size,
	TVoxelArray<float>& Distances,
	TVoxelArray<FVector3f>& Positions,
	const TVoxelArray<FVector3f>* VertexNormals = nullptr,
	TVoxelArray<FVector3f>* VoxelNormals = nullptr,
	int32* OutNumLeaks = nullptr);

}