// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "Landscape.h"
#include "LevelEditor.h"
#include "VoxelHeightmap.h"
#include "VoxelLandscapeConversion.h"

struct FVoxelLandscapeConversionEditor
{
	static void Convert()
	{
		TArray<UObject*> SelectedObjects;
		GEditor->GetSelectedActors()->GetSelectedObjects(ALandscape::StaticClass(), SelectedObjects);
		if (!ensure(SelectedObjects.Num() == 1))
		{
			return;
		}

		const ALandscape* Landscape = CastChecked<ALandscape>(SelectedObjects[0]);
		const ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();
		if (!ensure(LandscapeInfo))
		{
			return;
		}

		FVoxelObjectUtilities::CreateNewAsset_Deferred<UVoxelHeightmap>(
			Landscape->GetOutermost()->GetName(),
			"_" + Landscape->GetActorLabel() + "_VoxelHeightmap",
			[=](UVoxelHeightmap* Heightmap)
			{
				FVoxelLandscapeConversion::Convert(*Landscape, *Heightmap);
			});
	}
};

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterLandscapeConversion)
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetAllLevelViewportContextMenuExtenders().Add(MakeLambdaDelegate([](const TSharedRef<FUICommandList> CommandList, const TArray<AActor*> SelectedActors)
	{
		const TSharedRef<FExtender> Extender = MakeVoxelShared<FExtender>();

		if (SelectedActors.Num() == 1 && Cast<ALandscape>(SelectedActors[0]))
		{
			Extender->AddMenuExtension("ActorControl", EExtensionHook::After, CommandList, MakeLambdaDelegate([](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.BeginSection("ConvertLandscape", INVTEXT("Landscape"));

				MenuBuilder.AddMenuEntry(
					INVTEXT("Convert to voxel heightmap"),
					INVTEXT("Convert to voxel heightmap"),
					FSlateIcon(),
					FExecuteAction::CreateStatic(&FVoxelLandscapeConversionEditor::Convert),
					NAME_None,
					EUserInterfaceActionType::Button);

				MenuBuilder.EndSection();
			}));
		}

		return Extender;
	}));
}