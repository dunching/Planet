// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "Tools/UEdMode.h"
#include "InteractiveToolManager.h"
#include "BaseTools/SingleClickTool.h"
#include "VoxelActor.h"
#include "VoxelSculptEdMode.generated.h"

class FVoxelSculptCommands : public TVoxelCommands<FVoxelSculptCommands>
{
public:
	TSharedPtr<FUICommandInfo> Sculpt;

	virtual void RegisterCommands() override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UCLASS(Transient, Hidden, NotPlaceable, NotBlueprintType, NotBlueprintable)
class AVoxelPreviewActor : public AVoxelActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<AVoxelActor> TargetActor;
};

UCLASS()
class UVoxelSculptTool
	: public UInteractiveTool
	, public IHoverBehaviorTarget
	, public IClickDragBehaviorTarget
{
	GENERATED_BODY()

public:
	//~ Begin UInteractiveTool Interface
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;
	virtual void OnTick(float DeltaTime) override;
	//~ End UInteractiveTool Interface

	//~ Begin IHoverBehaviorTarget Interface
	virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
	virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override {}
	virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
	virtual void OnEndHover() override {}
	//~ End IHoverBehaviorTarget Interface

	//~ Begin IClickDragBehaviorTarget Interface
	virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
	virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
	virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
	virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
	virtual void OnTerminateDragSequence() override {}
	//~ End IClickDragBehaviorTarget Interface

	void UpdatePosition(const FInputDeviceRay& Position);
	bool DoEdit() const;

public:
	UPROPERTY(Transient)
	TObjectPtr<AVoxelPreviewActor> PreviewActor;

	bool bIsEditing = false;
	TOptional<FInputDeviceRay> LastRay;
};

UCLASS()
class UVoxelSculptToolBuilder : public USingleClickToolBuilder
{
	GENERATED_BODY()

public:
	//~ Begin USingleClickToolBuilder Interface
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override
	{
		return NewObject<UVoxelSculptTool>(SceneState.ToolManager);
	}
	//~ End USingleClickToolBuilder Interface
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UCLASS(config = EditorPerProjectUserSettings)
class UVoxelSculptEdModeSettings : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class UVoxelSculptEdMode : public UEdMode
{
	GENERATED_BODY()

public:
	UVoxelSculptEdMode();

	//~ Begin UEdMode Interface
	virtual void Enter() override;
	virtual void Exit() override;

	virtual void CreateToolkit() override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
	//~ End UEdMode Interface
};