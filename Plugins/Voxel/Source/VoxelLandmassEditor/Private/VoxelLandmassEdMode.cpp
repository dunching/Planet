// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelLandmassEdMode.h"
#include "VoxelActor.h"
#include "EditorModes.h"
#include "EditorModeManager.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "LevelViewportClickHandlers.h"

VOXEL_RUN_ON_STARTUP_EDITOR(ActivateLandmassEdMode)
{
	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditor.OnMapChanged().AddLambda([](UWorld* World, EMapChangeType ChangeType)
	{
		if (ChangeType == EMapChangeType::SaveMap)
		{
			return;
		}

		FVoxelSystemUtilities::DelayedCall([]
		{
			GLevelEditorModeTools().AddDefaultMode(GetDefault<UVoxelLandmassEdMode>()->GetID());
			GLevelEditorModeTools().ActivateDefaultMode();
		});
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelLandmassEdMode::UVoxelLandmassEdMode()
{
	Info = FEditorModeInfo(
		"VoxelLandmassEdMode",
		INVTEXT("VoxelLandmassEdMode"),
		FSlateIcon(),
		false,
		MAX_int32
	);

	SettingsClass = UVoxelLandmassEdModeSettings::StaticClass();
}

bool UVoxelLandmassEdMode::HandleClick(FEditorViewportClient* ViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	if (Click.GetKey() != EKeys::LeftMouseButton)
	{
		return false;
	}

	if (Click.GetEvent() != IE_Released &&
		Click.GetEvent() != IE_DoubleClick)
	{
		return false;
	}

	const HActor* ActorProxy = HitProxyCast<HActor>(HitProxy);
	if (!ActorProxy ||
		!ActorProxy->Actor)
	{
		return false;
	}

	if (!ActorProxy->Actor->IsA<AVoxelActor>())
	{
		return false;
	}

	FVector Start;
	FVector End;
	if (!ensure(FVoxelEditorUtilities::GetRayInfo(ViewportClient, Start, End)))
	{
		return false;
	}

	FHitResult HitResult;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_EngineTraceChannel6))
	{
		return false;
	}

	AActor* Actor = HitResult.GetActor();
	if (!ensure(Actor) ||
		!ensure(ViewportClient->IsLevelEditorClient()))
	{
		return false;
	}

	LevelViewportClickHandlers::ClickActor(
		static_cast<FLevelEditorViewportClient*>(ViewportClient),
		Actor,
		Click,
		true);

	return true;
}