// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlyingNavSystemTypes.h"

/*
* From
* Revelles, Jorge & Ureña, Carlos & Lastra, M. & Lenguajes, Dpt & Informaticos, Sistemas & Informatica, E.. (2000).
* An Efficient Parametric Algorithm for Octree Traversal.
* http://wscg.zcu.cz/wscg2000/Papers_2000/X31.pdf
*/

struct FRayIntersect
{
	const FCoord tx0;
	const FCoord tx1;
	const FCoord ty0;
	const FCoord ty1;
	const FCoord tz0;
	const FCoord tz1;

	FRayIntersect(const FCoord tx0, const FCoord tx1, const FCoord ty0, const FCoord ty1, const FCoord tz0, const FCoord tz1):
        tx0(tx0), tx1(tx1), ty0(ty0), ty1(ty1), tz0(tz0), tz1(tz1)
	{}

	bool Intersect() const
	{
		return FMath::Max3(tx0, ty0, tz0) < FMath::Min3(tx1, ty1, tz1);
	}

	bool IsValid(const FCoord MaxT) const
	{
		return (0 <= tx1 && tx0 <= MaxT && 0 <= ty1 && ty0 <= MaxT && 0.f <= tz1 && tz0 <= MaxT);
	}
};

struct FLYINGNAVSYSTEM_API FSVORaycast
{
	explicit FSVORaycast(const FSVOData& InNavData):
		NavData(InNavData.AsShared()),
		SignMask(0),
		FinalT(0),
		MaxT(0)
	{}
	
	FSVODataConstRef NavData;
	
protected:
	mutable int16 SignMask;
	mutable FCoord FinalT;
	mutable FCoord MaxT;

	static int32 FirstNode(const FRayIntersect& I);

	static int32 NextNode(const FCoord txM, const int32 X, const FCoord tyM, const int32 Y, const FCoord tzM, const int32 Z);

	bool RayIntersectSubNode(const FRayIntersect& I, const int32 ChildIdx, const FSVOLink LeafLink) const;
	// Returns if ray intersects blocking voxel in this node, with closest HitLocation
	bool RayIntersectNode(const FRayIntersect& I, const FSVOLink NodeLink) const;
	
public:
	/*
	 * Returns true if raycast hit filled octree node, providing the hit location
	 * false otherwise
	 */
	bool Raycast(const FVector& RayStart, const FVector& RayEnd, FVector& HitLocation) const
	{
		FVector Delta = (RayEnd - RayStart);
		if (Delta.IsNearlyZero())
		{
			return false;
		}
		
		FVector Origin = RayStart;
		const FCoord RayLength = Delta.Size();
		Delta = Delta / RayLength;

		const FBox RootBounds = FBox::BuildAABB(NavData->Centre, NavData->GetOctreeExtent());
		
		// Direction must be positive
		FVector Direction = Delta;

		FVector ReciprocalDirection;
		SignMask = 0;
		MaxT = RayLength;

		for (int32 i = 0; i < 3; i++)
		{
			// Take reciprocal
			if (FMath::IsNearlyZero(Direction[i]))
			{
				Direction[i] = DOUBLE_SMALL_NUMBER;
			}

			if (Direction[i] < 0.f)
			{
				Direction[i] = -Direction[i];
				Origin[i] = NavData->Centre[i] * 2.f - Origin[i];
				SignMask |= (1 << i);
			}
			
			ReciprocalDirection[i] = 1.f / Direction[i];
		}

		// Compute t values for root node;
		const FRayIntersect RootIntersect(
			(RootBounds.Min.X - Origin.X) * ReciprocalDirection.X,
			(RootBounds.Max.X - Origin.X) * ReciprocalDirection.X,
			(RootBounds.Min.Y - Origin.Y) * ReciprocalDirection.Y,
			(RootBounds.Max.Y - Origin.Y) * ReciprocalDirection.Y,
			(RootBounds.Min.Z - Origin.Z) * ReciprocalDirection.Z,
			(RootBounds.Max.Z - Origin.Z) * ReciprocalDirection.Z
        );
		
		if (RootIntersect.Intersect() && RayIntersectNode(RootIntersect, NavData->GetRootLink()))
		{
			HitLocation = RayStart + FinalT * Delta;
			return true;
		} else
		{
            return false;
		}
	}

	bool Raycast(const FVector& RayStart, const FVector& RayEnd) const
	{
		FVector HitPos;
		return Raycast(RayStart, RayEnd, HitPos);
    }
};