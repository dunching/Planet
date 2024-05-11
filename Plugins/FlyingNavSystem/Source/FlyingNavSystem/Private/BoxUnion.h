// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlyingNavSystemTypes.h"

// To deal with volume boundaries
#define FACE_INTERVAL_MARGIN 1.f

namespace FlyingNavSystem
{
	struct FFaceInterval
	{
		// Using flat boxes as interval defined axis-aligned quads
		FBox Intervals;

		EAxis::Type Axis;
		bool bCull = false;

		explicit FFaceInterval(EForceInit): Intervals(ForceInit), Axis(EAxis::None)
		{ }

		FFaceInterval(const FVector& InMin, const FVector& InMax, const EAxis::Type InAxis):
			Intervals(InMin, InMax),
			Axis(InAxis)
		{ }

		float GetCentreForAxis() const
		{
			return Intervals.GetCenter().GetComponentForAxis(Axis);
		}

		FFaceInterval AsMinSplit(const FFaceInterval& Other) const
		{
			// Make a copy
			FFaceInterval Split = *this;
			// Move max down to create lower segment
			Split.Intervals.Max.SetComponentForAxis(Other.Axis, Other.GetCentreForAxis() - FACE_INTERVAL_MARGIN);
			return Split;
		}

		FFaceInterval AsMaxSplit(const FFaceInterval& Other) const
		{
			// Make a copy
			FFaceInterval Split = *this;
			// Move min up to create upper segment
			Split.Intervals.Min.SetComponentForAxis(Other.Axis, Other.GetCentreForAxis() + FACE_INTERVAL_MARGIN);
			return Split;
		}

		void GetCorners(FVector Corners[]) const;

		bool IsDifferentAxes(const FFaceInterval& Other) const
		{
			return Axis != EAxis::None && Other.Axis != EAxis::None && Axis != Other.Axis;
		}

		bool Intersect(const FFaceInterval& Other) const
		{
			return Intervals.Intersect(Other.Intervals);
		}
		
		bool Intersect(const FBox& Other) const
		{
			return Intervals.Intersect(Other);
		}

		bool IsValid() const
		{
			return Intervals.IsValid && Axis != EAxis::None;
		}
	};

	struct FBoxUnion
	{
		explicit FBoxUnion(const TArray<FBox>& InBoxes, const float BoxShrink = 4.f);

		TNavStatArray<FCoord> ToQuadVertices() const;

	private:
		static void FacesForBox(TArray<FFaceInterval>& FaceIntervals, const FBox& Box);

		TArray<FFaceInterval> FaceIntervals;
		TArray<FBox> Boxes;
	};

	FRasterisableGeometry FindBoxUnion(const TArray<FBox>& Boxes);
}
