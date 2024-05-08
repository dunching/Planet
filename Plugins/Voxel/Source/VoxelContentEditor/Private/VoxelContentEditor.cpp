// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "ToolMenus.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/SVoxelAddContentDialog.h"

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterVoxelContentEditor)
{
	UToolMenu* ContextMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AddNewContextMenu");
	FToolMenuSection& ContextMenuSection = ContextMenu->FindOrAddSection("ContentBrowserGetContent");

	ContextMenuSection.AddDynamicEntry("GetVoxelContent", MakeLambdaDelegate([](FToolMenuSection& Section)
	{
		Section.AddMenuEntry(
			"GetVoxelContent",
			INVTEXT("Add Voxel Content"),
			INVTEXT("Add Voxel Plugin example content to this project"),
			FSlateIcon(FVoxelEditorStyle::GetStyleSetName(), "VoxelIcon"),
			FUIAction(FExecuteAction::CreateLambda(
			[]
			{
				const IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
				const TSharedPtr<SWindow> ParentWindow = MainFrame.GetParentWindow();

				if (!ensure(ParentWindow))
				{
					return;
				}

				FSlateApplication::Get().AddWindowAsNativeChild(SNew(SVoxelAddContentDialog), ParentWindow.ToSharedRef());
			})
		));
	}));
}