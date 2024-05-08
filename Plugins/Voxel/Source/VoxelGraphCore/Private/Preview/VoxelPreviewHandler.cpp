// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Preview/VoxelPreviewHandler.h"
#include "VoxelQuery.h"

TArray<const FVoxelPreviewHandler*> GVoxelPreviewHandlers;

VOXEL_RUN_ON_STARTUP_GAME(RegisterVoxelPreviewHandlersCleanup)
{
	GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
	{
		for (const FVoxelPreviewHandler* PreviewHandler : GVoxelPreviewHandlers)
		{
			FVoxelMemory::Delete(PreviewHandler);
		}
		GVoxelPreviewHandlers.Empty();
	});
}

const TArray<const FVoxelPreviewHandler*>& FVoxelPreviewHandler::GetHandlers()
{
	if (GVoxelPreviewHandlers.Num() == 0)
	{
		for (UScriptStruct* Struct : GetDerivedStructs<FVoxelPreviewHandler>())
		{
			GVoxelPreviewHandlers.Add(TVoxelInstancedStruct<FVoxelPreviewHandler>(Struct).Release());
		}
	}

	return GVoxelPreviewHandlers;
}

void FVoxelPreviewHandler::BuildStats(const FAddStat& AddStat)
{
	AddStat(
		"Position",
		"The position currently being previewed",
		MakeWeakPtrLambda(this, [this]
		{
			return CurrentPosition.ToString();
		}));
}

void FVoxelPreviewHandler::UpdateStats(const FVector2D& MousePosition)
{
	const FMatrix PixelToWorld = QueryContext->RuntimeInfo->GetLocalToWorld().Get_NoDependency();
	CurrentPosition = PixelToWorld.TransformPosition(FVector(MousePosition.X, MousePosition.Y, 0.f));
}