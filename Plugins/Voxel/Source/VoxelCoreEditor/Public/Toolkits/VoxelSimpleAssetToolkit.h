// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelToolkit.h"
#include "AdvancedPreviewScene.h"
#include "VoxelSimpleAssetToolkit.generated.h"

class SViewportToolBar;
class SVoxelSimpleAssetEditorViewport;

USTRUCT()
struct VOXELCOREEDITOR_API FVoxelSimpleAssetToolkit : public FVoxelToolkit
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

public:
	//~ Begin FVoxelToolkit Interface
	virtual void Initialize() override;
	virtual TSharedPtr<FTabManager::FLayout> GetLayout() const override;
	virtual TSharedPtr<SWidget> GetMenuOverlay() const override;
	virtual void RegisterTabs(FRegisterTab RegisterTab) override;

	virtual void Tick() override;

	virtual void SaveDocuments() override;
	virtual void LoadDocuments() override;

	virtual void PostEditChange(const FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End FVoxelToolkit Interface

	IDetailsView& GetDetailsView() const
	{
		return *PrivateDetailsView;
	}
	FAdvancedPreviewScene& GetPreviewScene() const
	{
		return *PrivatePreviewScene;
	}

	virtual void SetupPreview() {}
	virtual void UpdatePreview() {}
	virtual void DrawPreview(const FSceneView* View, FPrimitiveDrawInterface* PDI) {}
	virtual void DrawPreviewCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) {}

	virtual void PopulateToolBar(const TSharedPtr<SHorizontalBox>& ToolbarBox, const TSharedPtr<SViewportToolBar>& ParentToolBarPtr) {}
	virtual TSharedRef<SWidget> PopulateToolBarShowMenu() { return SNullWidget::NullWidget; }

	virtual bool ShowFloor() const { return true; }
	virtual bool SaveCameraPosition() const { return false; }
	virtual bool ShowFullTransformsToolbar() const { return false; }
	virtual FRotator GetInitialViewRotation() const { return FRotator(-20.0f, 45.0f, 0.f); }
	virtual TOptional<float> GetInitialViewDistance() const { return {}; }

	void DrawThumbnail(FViewport& InViewport);

protected:
	static const FName DetailsTabId;
	static const FName ViewportTabId;

private:
	bool bPreviewQueued = false;
	FString QueuedStatsText;

	bool bCaptureThumbnail = false;

	TSharedPtr<IDetailsView> PrivateDetailsView;
	TSharedPtr<FAdvancedPreviewScene> PrivatePreviewScene;
	TSharedPtr<SVoxelSimpleAssetEditorViewport> Viewport;

protected:
	void UpdateStatsText(const FString& Message);
	void BindToggleCommand(const TSharedPtr<FUICommandInfo>& UICommandInfo, bool& bValue);
	void SetFloorScale(const FVector& Scale) const;
	void CaptureThumbnail();

private:
	FObjectProperty* GetTextureProperty() const;
};