// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelContentEditorModule.h"
#include "Interfaces/IMainFrameModule.h"
#include "Widgets/SVoxelAddContentDialog.h"

class FVoxelContentEditorModule : public IVoxelContentEditorModule
{
public:
	virtual void ShowContent() override
	{
		const IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		const TSharedPtr<SWindow> ParentWindow = MainFrame.GetParentWindow();

		if (!ensure(ParentWindow))
		{
			return;
		}

		FSlateApplication::Get().AddWindowAsNativeChild(SNew(SVoxelAddContentDialog), ParentWindow.ToSharedRef());
	}
};
IMPLEMENT_MODULE(FVoxelContentEditorModule, VoxelContentEditor);