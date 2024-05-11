// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/LineBatchComponent.h"
#include "HAL/ThreadSafeBool.h"
#include "PathBatchComponent.generated.h"

struct FFlyingNavigationPath;

struct FBatchedPath
{
	explicit FBatchedPath(const FColor InColor) :
		Color(InColor),
		ID(++NextUniqueId)
	{}

	void ApplyWorldOffset(const FVector& InOffset);

	TArray<FVector> PathPoints;
	const FColor Color;
	const int32 ID;

private:
	static int32 NextUniqueId;
};

/**
* Adds extra functionality for drawing persistent paths
*/
UCLASS(MinimalAPI)
class UPathBatchComponent : public ULineBatchComponent
{
	GENERATED_BODY()

public:
	UPathBatchComponent();

	// Adds a path to be drawn, returning the index of that path (so it can be removed later) (thread safe)
	int32 AddPath(const FFlyingNavigationPath& NavPath, const FVector& PathOffset, const FColor PathColor, const int32 Index = INDEX_NONE);

	// Remove path by index (thread safe)
	void RemovePath(const int32 Index);

	//~ Begin UActorComponent Interface.
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override {} // Don't remove lines
	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;
	//~ End UActorComponent Interface.

protected:
	TArray<FBatchedPath> BatchedPaths;
	FThreadSafeBool bQueuedRedraw;
	FCriticalSection BatchedPathsLock;

	// Flush and draw paths, threadsafe
	void Redraw();

	// Draw single path, called from game thread
	void DrawPath(const FBatchedPath& BatchedPath);
};
