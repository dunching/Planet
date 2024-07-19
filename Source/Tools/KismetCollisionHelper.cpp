
#include "KismetCollisionHelper.h"

#include "KismetGravityLibrary.h"

static TAutoConsoleVariable<int32> CollisionHelper(
	TEXT("CollisionHelper.DrawDebug"),
	0,
	TEXT("")
	TEXT(" default: 0"));

TArray<FHitResult> UKismetCollisionHelper::OverlapMultiSectorByObjectType(
	const UWorld* World,
	const FVector& Start,
	const FVector& End,
	int32 Angle,
	int32 LineNum,
	const FCollisionObjectQueryParams& ObjectQueryParams,
	const FCollisionQueryParams& Params
)
{
	TArray<struct FHitResult> OutHits;

	const auto SingleAnlge = Angle / (LineNum - 1);
	const auto Dir = End - Start;
	const auto Mid = LineNum / 2;
	const auto OffsetAngle = (LineNum % 2 == 0) ? (SingleAnlge / 2) : 0;

	for (int32 Index = 0; Index < LineNum; Index++)
	{
		const auto CurrentAngle = (SingleAnlge * (Index - Mid)) + OffsetAngle;
		const auto CurrentEndPt =
			Start + Dir.RotateAngleAxis(CurrentAngle, -UKismetGravityLibrary::GetGravity(Start));

#ifdef WITH_EDITOR
		if (CollisionHelper.GetValueOnGameThread())
		{
			DrawDebugLine(World, Start, CurrentEndPt, FColor::Red, false, 3);
		}
#endif
		TArray<FHitResult> CurrentOutHits;

		if (World->LineTraceMultiByObjectType(CurrentOutHits, Start, CurrentEndPt, ObjectQueryParams, Params))
		{
			OutHits.Append(CurrentOutHits);
		}
	}

	return OutHits;
}

