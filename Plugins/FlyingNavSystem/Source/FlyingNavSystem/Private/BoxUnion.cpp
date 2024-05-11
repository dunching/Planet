// Copyright Ben Sutherland 2024. All rights reserved.

#include "BoxUnion.h"

void FlyingNavSystem::FFaceInterval::GetCorners(FVector Corners[]) const
{
	Corners[0] = Intervals.Min;
	switch (Axis)
	{
	case EAxis::X:
		Corners[1] = FVector(Intervals.Min.X, Intervals.Min.Y, Intervals.Max.Z);
		Corners[2] = FVector(Intervals.Min.X, Intervals.Max.Y, Intervals.Max.Z);
		Corners[3] = FVector(Intervals.Min.X, Intervals.Max.Y, Intervals.Min.Z);
		break;
	case EAxis::Y:
		Corners[1] = FVector(Intervals.Min.X, Intervals.Min.Y, Intervals.Max.Z);
		Corners[2] = FVector(Intervals.Max.X, Intervals.Min.Y, Intervals.Max.Z);
		Corners[3] = FVector(Intervals.Max.X, Intervals.Min.Y, Intervals.Min.Z);
		break;
	case EAxis::Z:
		Corners[1] = FVector(Intervals.Min.X, Intervals.Max.Y, Intervals.Min.Z);
		Corners[2] = FVector(Intervals.Max.X, Intervals.Max.Y, Intervals.Min.Z);
		Corners[3] = FVector(Intervals.Max.X, Intervals.Min.Y, Intervals.Min.Z);
		break;
	case EAxis::None:
		break;
	}
}

FlyingNavSystem::FBoxUnion::FBoxUnion(const TArray<FBox>& InBoxes, const float BoxShrink)
{
	if (InBoxes.Num() == 0)
	{
		return;
	}

	// Shrink Boxes so the resulting triangles aren't culled by geometry gathering
	Boxes.Reserve(InBoxes.Num());
	for (const FBox& Box : InBoxes)
	{
		FBox ShrunkBox = Box.ExpandBy(-BoxShrink);
		if (ShrunkBox.GetVolume() > 0)
		{
			Boxes.Add(ShrunkBox);
		}
	}
	
	FacesForBox(FaceIntervals, Boxes[0]);

	TArray<FFaceInterval> BoxFaceIntervals;
	BoxFaceIntervals.Reserve(16);
	for (int32 i = 1; i < Boxes.Num(); i++)
	{
		// Add box to union
		BoxFaceIntervals.Reset();
		FacesForBox(BoxFaceIntervals, Boxes[i]);

		// Subdivide based on intersections between new box and existing union
		for (int32 j = 0; j < FaceIntervals.Num(); j++)
		{
			for (int32 k = 0; k < BoxFaceIntervals.Num(); k++)
			{
				if (FaceIntervals[j].IsDifferentAxes(BoxFaceIntervals[k]) &&
					FaceIntervals[j].Intersect(BoxFaceIntervals[k]))
				{
					if (!FaceIntervals[j].bCull)
					{
						FaceIntervals.Add(FaceIntervals[j].AsMinSplit(BoxFaceIntervals[k]));
						FaceIntervals.Add(FaceIntervals[j].AsMaxSplit(BoxFaceIntervals[k]));
						FaceIntervals[j].bCull = true;
					}
					
					if (!BoxFaceIntervals[k].bCull)
					{
						BoxFaceIntervals.Add(BoxFaceIntervals[k].AsMinSplit(FaceIntervals[j]));
						BoxFaceIntervals.Add(BoxFaceIntervals[k].AsMaxSplit(FaceIntervals[j]));
						BoxFaceIntervals[k].bCull = true;
					}
				}
			}
		}

		// Move over valid faces
		for (const FFaceInterval& FaceInterval : BoxFaceIntervals)
		{
			if (!FaceInterval.bCull && FaceInterval.IsValid())
			{
				FaceIntervals.Add(FaceInterval);
			}
		}
	}

	// Shrink boxes slightly so the box intersection test doesn't include its faces
	for (FBox& Box : Boxes)
	{
		Box = Box.ExpandBy(-FACE_INTERVAL_MARGIN);
	}
	
	// Cull internal faces
	TArray<FFaceInterval> NewFaceIntervals;
	NewFaceIntervals.Reserve(FaceIntervals.Num()); // Over reserve
	for (FFaceInterval& FaceInterval : FaceIntervals)
	{
		if (!FaceInterval.bCull && FaceInterval.IsValid())
		{
			bool bExternal = true;
			for (const FBox& Box : Boxes)
			{
				if (FaceInterval.Intersect(Box))
				{
					bExternal = false;
					break;
				}
			}
			if (bExternal)
			{
				NewFaceIntervals.Add(FaceInterval);
			}
		}
	}
	
	FaceIntervals = MoveTemp(NewFaceIntervals);
}

TNavStatArray<FCoord> FlyingNavSystem::FBoxUnion::ToQuadVertices() const
{
	TNavStatArray<FCoord> Vertices;
	Vertices.Reserve(FaceIntervals.Num() * 12); // 4 verts per face, 3 coords per vert, 12 coords

	for (const FFaceInterval& FaceInterval : FaceIntervals)
	{
		FVector Corners[4];
		FaceInterval.GetCorners(Corners);
		// Copy over coords
		Vertices.Append(reinterpret_cast<FCoord*>(Corners), 12);
	}
			
	return Vertices;
}

void FlyingNavSystem::FBoxUnion::FacesForBox(TArray<FFaceInterval>& FaceIntervals, const FBox& Box)
{
	FaceIntervals.AddUninitialized(6);
	FaceIntervals.Last(5) = FFaceInterval(
		Box.Min,
		FVector(Box.Min.X, Box.Max.Y, Box.Max.Z),
		EAxis::X);
	FaceIntervals.Last(4) = FFaceInterval(
		FVector(Box.Max.X, Box.Min.Y, Box.Min.Z),
		Box.Max,
		EAxis::X);
	FaceIntervals.Last(3) = FFaceInterval(
		Box.Min,
		FVector(Box.Max.X, Box.Min.Y, Box.Max.Z),
		EAxis::Y);
	FaceIntervals.Last(2) = FFaceInterval(
		FVector(Box.Min.X, Box.Max.Y, Box.Min.Z),
		Box.Max,
		EAxis::Y);
	FaceIntervals.Last(1) = FFaceInterval(
		Box.Min,
		FVector(Box.Max.X, Box.Max.Y, Box.Min.Z),
		EAxis::Z);
	FaceIntervals.Last(0) = FFaceInterval(
		FVector(Box.Min.X, Box.Min.Y, Box.Max.Z),
		Box.Max,
		EAxis::Z);
}

FRasterisableGeometry FlyingNavSystem::FindBoxUnion(const TArray<FBox>& Boxes)
{
	static constexpr int32 QuadIndices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	FRasterisableGeometry FRasterisableGeometry;
	FRasterisableGeometry.VertexBuffer = FBoxUnion(Boxes).ToQuadVertices();
	const int32 NumVerts = FRasterisableGeometry.VertexBuffer.Num() / 3;
	const int32 NumQuads = NumVerts / 4;
	FRasterisableGeometry.IndexBuffer.Reserve(NumQuads * 6);
	
	for (int32 i = 0; i < NumQuads; i++)
	{
		for (int32 j = 0; j < 6; j++)
		{
			FRasterisableGeometry.IndexBuffer.Add(i*4 + QuadIndices[j]);
		}
	}
	
	return FRasterisableGeometry;
}
