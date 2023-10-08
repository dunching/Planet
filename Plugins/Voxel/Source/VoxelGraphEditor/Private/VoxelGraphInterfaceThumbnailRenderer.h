// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphInterfaceToolkit.h"
#include "VoxelGraphInterfaceThumbnailRenderer.generated.h"

class VOXELGRAPHEDITOR_API FVoxelGraphThumbnailScene : public FVoxelThumbnailScene
{
public:
	TWeakObjectPtr<AVoxelActor> VoxelActor;

	FVoxelGraphThumbnailScene();

	//~ Begin FVoxelThumbnailScene Interface
	virtual FBoxSphereBounds GetBounds() const override;
	//~ End FVoxelThumbnailScene Interface
};

UCLASS()
class VOXELGRAPHEDITOR_API UVoxelGraphInterfaceThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
	GENERATED_BODY()

public:
	//~ Begin UDefaultSizedThumbnailRenderer Interface
	virtual void BeginDestroy() override;
	virtual bool CanVisualizeAsset(UObject* Object) override;

	virtual void Draw(
		UObject* Object,
		int32 X,
		int32 Y,
		uint32 Width,
		uint32 Height,
		FRenderTarget* Viewport,
		FCanvas* Canvas,
		bool bAdditionalViewFamily) override;

	virtual EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const override
	{
		return EThumbnailRenderFrequency::OnPropertyChange;
	}
	//~ End UDefaultSizedThumbnailRenderer Interface

private:
	TSharedPtr<FVoxelGraphThumbnailScene> ThumbnailScene;
};