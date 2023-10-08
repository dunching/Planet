// Copyright Voxel Plugin, Inc. All Rights Reserved.

/**
 * This code is a heavily modified version of https://github.com/christopherbatty/SDFGen
 *
 * License is as follows:
 * The MIT License (MIT)
 *
 * Copyright (c) 2015, Christopher Batty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "VoxelMeshVoxelizer.h"

namespace Voxel::MeshVoxelizer
{

// Calculate twice signed area of triangle (0,0)-(A.X,A.Y)-(B.X,B.Y)
// Return an SOS-determined sign (-1, +1, or 0 only if it's a truly degenerate triangle)
inline int32 Orientation(
	const FVector2d& A,
	const FVector2d& B,
	double& TwiceSignedArea)
{
	TwiceSignedArea = A.Y * B.X - A.X * B.Y;
	if (TwiceSignedArea > 0) return 1;
	else if (TwiceSignedArea < 0) return -1;
	else if (B.Y > A.Y) return 1;
	else if (B.Y < A.Y) return -1;
	else if (A.X > B.X) return 1;
	else if (A.X < B.X) return -1;
	else return 0; // Only true when A.X == B.X and A.Y == B.Y
}

// Robust test of (x0,y0) in the triangle (x1,y1)-(x2,y2)-(x3,y3)
// If true is returned, the barycentric coordinates are set in a,b,c.
inline bool PointInTriangle2D(
	const FVector2d& Point,
	FVector2d A,
	FVector2d B,
	FVector2d C,
	double& AlphaA, double& AlphaB, double& AlphaC)
{
	A -= Point;
	B -= Point;
	C -= Point;

	const int32 SignA = Orientation(B, C, AlphaA);
	if (SignA == 0) return false;

	const int32 SignB = Orientation(C, A, AlphaB);
	if (SignB != SignA) return false;

	const int32 SignC = Orientation(A, B, AlphaC);
	if (SignC != SignA) return false;

	const double Sum = AlphaA + AlphaB + AlphaC;
	checkSlow(Sum != 0); // if the SOS signs match and are non-zero, there's no way all of a, b, and c are zero.
	AlphaA /= Sum;
	AlphaB /= Sum;
	AlphaC /= Sum;

	return true;
}

void Voxelize(
	const FVoxelMeshVoxelizerSettings& Settings,
	const TVoxelArray<FVector3f>& Vertices,
	const TVoxelArray<int32>& Indices,
	const FVector3f& Origin,
	const FIntVector& Size,
	TVoxelArray<float>& Distances,
	TVoxelArray<FVector3f>& Positions,
	const TVoxelArray<FVector3f>* VertexNormals,
	TVoxelArray<FVector3f>* VoxelNormals,
	int32* OutNumLeaks)
{
	VOXEL_FUNCTION_COUNTER();

	const bool bComputeNormals = VertexNormals != nullptr;
	check(bComputeNormals == (VoxelNormals != nullptr));
	check(!VertexNormals || VertexNormals->Num() == Vertices.Num());

	const int64 NumVoxels = int64(Size.X) * int64(Size.Y) * int64(Size.Z);
	if (!ensure(NumVoxels < MAX_int32))
	{
		return;
	}

	FVoxelUtilities::SetNumFast(Distances, NumVoxels);
	FVoxelUtilities::SetNumFast(Positions, NumVoxels);

	FVoxelUtilities::SetAll(Distances, MAX_flt);
	FVoxelUtilities::SetAll(Positions, FVector3f(MAX_flt));

	if (bComputeNormals)
	{
		FVoxelUtilities::SetNumFast(*VoxelNormals, NumVoxels);
		FVoxelUtilities::Memzero(*VoxelNormals);
	}

	TVoxelArray<int32> IntersectionCount;
	FVoxelUtilities::SetNumFast(IntersectionCount, NumVoxels);
	FVoxelUtilities::Memzero(IntersectionCount);

	// Find the axis mappings based on the sweep direction
	int32 IndexI;
	int32 IndexJ;
	int32 IndexK;
	switch (Settings.SweepDirection)
	{
	default: ensure(false);
	case EVoxelAxis::X: IndexI = 1; IndexJ = 2; IndexK = 0; break;
	case EVoxelAxis::Y: IndexI = 2; IndexJ = 0; IndexK = 1; break;
	case EVoxelAxis::Z: IndexI = 0; IndexJ = 1; IndexK = 2; break;
	}

	// We begin by initializing distances near the mesh, and figuring out intersection counts
	{
		VOXEL_SCOPE_COUNTER("Intersections");
		for (int32 TriangleIndex = 0; TriangleIndex < Indices.Num(); TriangleIndex += 3)
		{
			VOXEL_SCOPE_COUNTER("Process triangle");

			const int32 IndexA = Indices[TriangleIndex + 0];
			const int32 IndexB = Indices[TriangleIndex + 1];
			const int32 IndexC = Indices[TriangleIndex + 2];

			const FVector3f& VertexA = Vertices[IndexA];
			const FVector3f& VertexB = Vertices[IndexB];
			const FVector3f& VertexC = Vertices[IndexC];

			const auto ToVoxelSpace = [&](const FVector3f& Value)
			{
				return Value - Origin;
			};
			const auto FromVoxelSpace = [&](const FVector3f& Value)
			{
				return Value + Origin;
			};

			const FVector3f VoxelVertexA = ToVoxelSpace(VertexA);
			const FVector3f VoxelVertexB = ToVoxelSpace(VertexB);
			const FVector3f VoxelVertexC = ToVoxelSpace(VertexC);

			const FVector3f MinVoxelVertex = FVoxelUtilities::ComponentMin3(VoxelVertexA, VoxelVertexB, VoxelVertexC);
			const FVector3f MaxVoxelVertex = FVoxelUtilities::ComponentMax3(VoxelVertexA, VoxelVertexB, VoxelVertexC);

			{
				VOXEL_SCOPE_COUNTER("Compute distance");

				const FIntVector Start = FVoxelUtilities::Clamp(FVoxelUtilities::FloorToInt(MinVoxelVertex), FIntVector(0), Size - 1);
				const FIntVector End = FVoxelUtilities::Clamp(FVoxelUtilities::CeilToInt(MaxVoxelVertex), FIntVector(0), Size - 1);

				// Do distances nearby
				for (int32 Z = Start.Z; Z <= End.Z; Z++)
				{
					for (int32 Y = Start.Y; Y <= End.Y; Y++)
					{
						for (int32 X = Start.X; X <= End.X; X++)
						{
							const FVector3f Position = FromVoxelSpace(FVector3f(X, Y, Z));

							float AlphaA;
							float AlphaB;
							float AlphaC;
							const float Distance = FMath::Sqrt(PointTriangleDistanceSquared(Position, VertexA, VertexB, VertexC, AlphaA, AlphaB, AlphaC));

							const int32 Index = FVoxelUtilities::Get3DIndex<int32>(Size, X, Y, Z);
							if (Distance < Distances[Index])
							{
								Distances[Index] = Distance;
								Positions[Index] = AlphaA * VoxelVertexA + AlphaB * VoxelVertexB + AlphaC * VoxelVertexC;

								if (bComputeNormals)
								{
									(*VoxelNormals)[Index] = (
										AlphaA * (*VertexNormals)[IndexA] +
										AlphaB * (*VertexNormals)[IndexB] +
										AlphaC * (*VertexNormals)[IndexC]).GetSafeNormal();
								}
							}
						}
					}
				}
			}

			{
				VOXEL_SCOPE_COUNTER("Compute intersections");

				const FIntVector Start = FVoxelUtilities::Clamp(FVoxelUtilities::CeilToInt(MinVoxelVertex), FIntVector(0), Size - 1);
				const FIntVector End = FVoxelUtilities::Clamp(FVoxelUtilities::FloorToInt(MaxVoxelVertex), FIntVector(0), Size - 1);

				// Do intersection counts. Make sure to follow SweepDirection!
				FIntVector Position;
				for (int32 I = Start[IndexI]; I <= End[IndexI]; I++)
				{
					Position[IndexI] = I;
					for (int32 J = Start[IndexJ]; J <= End[IndexJ]; J++)
					{
						Position[IndexJ] = J;

						const auto Get2D = [&](const FVector3f& V) { return FVector2d(V[IndexI], V[IndexJ]); };

						double AlphaA;
						double AlphaB;
						double AlphaC;
						if (!PointInTriangle2D(FVector2d(I, J), Get2D(VoxelVertexA), Get2D(VoxelVertexB), Get2D(VoxelVertexC), AlphaA, AlphaB, AlphaC))
						{
							continue;
						}

						const float K = AlphaA * VoxelVertexA[IndexK] + AlphaB * VoxelVertexB[IndexK] + AlphaC * VoxelVertexC[IndexK]; // Intersection K coordinate
						Position[IndexK] = FMath::Clamp(Settings.bReverseSweep ? FMath::FloorToInt(K) : FMath::CeilToInt(K), 0, Size[IndexK] - 1);
						IntersectionCount[FVoxelUtilities::Get3DIndex<int32>(Size, Position)]++;
					}
				}
			}
		}
	}

	int32 NumLeaks = 0;
	{
		VOXEL_SCOPE_COUNTER("Compute Signs");

		// Then figure out signs (inside/outside) from intersection counts
		FIntVector Position;
		for (int32 I = 0; I < Size[IndexI]; I++)
		{
			Position[IndexI] = I;
			for (int32 J = 0; J < Size[IndexJ]; J++)
			{
				Position[IndexJ] = J;

				// Compute the number of intersections, and skip it if it's a leak
				if (Settings.bHideLeaks)
				{
					int32 Count = 0;
					for (int32 K = 0; K < Size[IndexK]; K++)
					{
						Position[IndexK] = K;
						Count += IntersectionCount[FVoxelUtilities::Get3DIndex<int32>(Size, Position)];
					}

					if (Settings.bWatertight)
					{
						if (Count % 2 == 1)
						{
							// For watertight meshes, we're expecting to come in and out of the mesh
							NumLeaks++;
							continue;
						}
					}
					else
					{
						if (Count == 0)
						{
							// For other meshes, only skip when there was no hit
							NumLeaks++;
							continue;
						}
					}
				}
				// If we are not watertight, start inside (unless we're reverse)
				int32 Count = (!Settings.bWatertight && !Settings.bReverseSweep) ? 1 : 0;

				for (int32 K = 0; K < Size[IndexK]; K++)
				{
					Position[IndexK] = Settings.bReverseSweep ? Size[IndexK] - 1 - K : K;

					const int32 Index = FVoxelUtilities::Get3DIndex<int32>(Size, Position);

					Count += IntersectionCount[Index];
					if (Count % 2 == 1)
					{
						// If parity of intersections so far is odd, we are inside the mesh
						Distances[Index] *= -1;
					}
				}
			}
		}
	}

	if (OutNumLeaks)
	{
		*OutNumLeaks = NumLeaks;
	}
}

}