// Copyright Ben Sutherland 2024. All rights reserved.

#include "PathBatchComponent.h"
#include "NavigationSystem.h"
#include "FlyingNavigationData.h"
#include "Async/Async.h"

int32 FBatchedPath::NextUniqueId = 0;

void FBatchedPath::ApplyWorldOffset(const FVector& InOffset)
{
	for (FVector& PathPoint : PathPoints) { PathPoint += InOffset; }
}

UPathBatchComponent::UPathBatchComponent() : bQueuedRedraw(false) {}

int32 UPathBatchComponent::AddPath(const FFlyingNavigationPath& NavPath, const FVector& PathOffset, const FColor PathColor, const int32 Index)
{
	FScopeLock DataLock(&BatchedPathsLock);

	FBatchedPath* BatchedPath = nullptr;

	if (Index != INDEX_NONE)
	{
		BatchedPath = BatchedPaths.FindByPredicate([Index](const FBatchedPath& Path)
		{
			return Path.ID == Index;
		});
	}

	if (BatchedPath == nullptr)
	{
		BatchedPath = new(BatchedPaths) FBatchedPath(PathColor);
	}

	// Copy over path points
	TArray<FVector>& PathPoints = BatchedPath->PathPoints;
	FlyingNavSystem::ConvertNavPathPointsToVector(NavPath.GetPathPoints(), PathPoints);

	const int32 NumPathPoints = PathPoints.Num();
	if (NumPathPoints > 0)
	{
		// Apply path offset
		for (FVector& PathPoint : PathPoints) { PathPoint += PathOffset; }

		// Add slight offset to last point so it doesn't disappear from the viewer's perspective
		PathPoints[NumPathPoints - 1].Z += 0.1f;
	}

	Redraw();

	return BatchedPath->ID;
}

void UPathBatchComponent::RemovePath(const int32 Index)
{
	if (Index == INDEX_NONE) { return; }

	FScopeLock DataLock(&BatchedPathsLock);
	bool bModified = false;
	for (int32 i = 0; i < BatchedPaths.Num(); i++)
	{
		if (BatchedPaths[i].ID == Index)
		{
			BatchedPaths.RemoveAtSwap(i);
			bModified = true;
			break;
		}
	}

	if (bModified) { Redraw(); }
}

void UPathBatchComponent::ApplyWorldOffset(const FVector& InOffset, bool bWorldShift)
{
	Super::ApplyWorldOffset(InOffset, bWorldShift);

	FScopeLock DataLock(&BatchedPathsLock);
	for (FBatchedPath& Path : BatchedPaths)
	{
		Path.ApplyWorldOffset(InOffset);
	}
}

void UPathBatchComponent::Redraw()
{
	if (!bQueuedRedraw)
	{
		bQueuedRedraw = true;
		// Draw on the game thread
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			Flush();

			FScopeLock DataLock(&BatchedPathsLock);
			for (const FBatchedPath& Path : BatchedPaths)
			{
				DrawPath(Path);
			}

			bQueuedRedraw = false;
		});
	}
}

void UPathBatchComponent::DrawPath(const FBatchedPath& BatchedPath)
{
	const TArray<FVector>& PathPoints = BatchedPath.PathPoints;
	const FColor& PathColor = BatchedPath.Color;
	const int32 NumPathVerts = PathPoints.Num();

	for (int32 PointIdx = 0; PointIdx < NumPathVerts - 1; ++PointIdx)
	{
		// Draw box at path point
		DrawSolidBox(
			FBox::BuildAABB(PathPoints[PointIdx], NavigationDebugDrawing::PathNodeBoxExtent),
			FTransform::Identity,
			PathColor,
			SDPG_World,
			-1.f);

		// Draw line from this point to the next
		DrawLine(
			PathPoints[PointIdx],
			PathPoints[PointIdx + 1],
			PathColor,
			SDPG_World,
			NavigationDebugDrawing::PathLineThickness,
			-1.f);
	}

	// Draw last point
	if (NumPathVerts > 0)
	{
		DrawBox(
			FBox::BuildAABB(PathPoints[NumPathVerts - 1], NavigationDebugDrawing::PathNodeBoxExtent),
			FMatrix::Identity,
			PathColor,
			SDPG_World);
	}
}
