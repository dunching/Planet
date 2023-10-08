// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelTransformRef.h"
#include "VoxelGraphNodeRef.h"

class UVoxelGraph;
class SVoxelGraphPreviewImage;
class SVoxelGraphPreviewScale;
class SVoxelGraphPreviewRuler;
class SVoxelGraphPreviewStats;
class SVoxelGraphPreviewDepthSlider;
struct FVoxelPreviewHandler;

class SVoxelGraphPreview : public SCompoundWidget
{
public:
	TWeakObjectPtr<UVoxelGraph> WeakGraph;

	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UVoxelGraph*, Graph);
	};

	void Construct(const FArguments& Args);

	void QueueUpdate()
	{
		bUpdateQueued = true;
	}

	TSharedRef<SWidget> GetPreviewStats() const;
	void AddReferencedObjects(FReferenceCollector& Collector) const;

	//~ Begin SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~ End SWidget Interface

private:
	class FTransformProvider : public IVoxelTransformProvider
	{
	public:
		FMatrix Transform = FMatrix::Identity;

		virtual FName GetName() const override
		{
			return "Preview";
		}
		virtual FMatrix GetTransform() const override
		{
			return Transform;
		}
	};
	const TSharedRef<FTransformProvider> TransformProvider = MakeVoxelShared<FTransformProvider>();
	const FVoxelTransformRef TransformRef = FVoxelTransformRef::Make(TransformProvider);

	bool bUpdateQueued = false;
	double ProcessingStartTime = 0;
	FString Message;

	TSharedPtr<FVoxelPreviewHandler> PreviewHandler;

	TSharedPtr<SVoxelGraphPreviewStats> PreviewStats;
	TSharedPtr<SVoxelGraphPreviewImage> PreviewImage;
	TSharedPtr<SVoxelGraphPreviewRuler> PreviewRuler;
	TSharedPtr<SVoxelGraphPreviewDepthSlider> DepthSlider;

	FVector2D MousePosition = FVector2D::ZeroVector;

	bool bLockCoordinatePending = false;
	bool bIsCoordinateLocked = false;
	FVector LockedCoordinate_WorldSpace = FVector::ZeroVector;

	void Update();
	void UpdateStats();
	FMatrix GetPixelToWorld() const;
};