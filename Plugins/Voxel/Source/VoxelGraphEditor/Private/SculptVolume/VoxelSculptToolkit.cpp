// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSculptToolkit.h"
#include "VoxelRuntimeGraph.h"
#include "VoxelSculptEdMode.h"
#include "Widgets/SVoxelGraphSelector.h"
#include "Toolkits/AssetEditorModeUILayer.h"
#include "Sculpt/VoxelEditSculptSurfaceExecNode.h"

void FVoxelSculptToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);

	SetCurrentPalette("Tools");
}

FName FVoxelSculptToolkit::GetToolkitFName() const
{
	return GetDefault<UVoxelSculptEdMode>()->GetModeInfo().ID;
}

FText FVoxelSculptToolkit::GetBaseToolkitName() const
{
	return GetDefault<UVoxelSculptEdMode>()->GetModeInfo().Name;
}

FText FVoxelSculptToolkit::GetActiveToolDisplayName() const
{
	return INVTEXT("FVoxelSculptToolkit::GetActiveToolDisplayName");
}

FText FVoxelSculptToolkit::GetActiveToolMessage() const
{
	return INVTEXT("FVoxelSculptToolkit::GetActiveToolMessage");
}

void FVoxelSculptToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add("Tools");
}

bool FVoxelSculptToolkit::HasIntegratedToolPalettes() const
{
	return false;
}

void FVoxelSculptToolkit::RequestModeUITabs()
{
	const TSharedPtr<FAssetEditorModeUILayer> PinnedModeUILayerPtr = ModeUILayer.Pin();
	if (!PinnedModeUILayerPtr)
	{
		return;
	}

	PrimaryTabInfo.OnSpawnTab = FOnSpawnTab::CreateSP(SharedThis(this), &FVoxelSculptToolkit::CreatePrimaryModePanel);
	PrimaryTabInfo.TabLabel = INVTEXT("Mode Toolbox");
	PrimaryTabInfo.TabTooltip = INVTEXT("Open the Modes tab, which contains the active editor mode's settings.");
	PinnedModeUILayerPtr->SetModePanelInfo(UAssetEditorUISubsystem::TopLeftTabID, PrimaryTabInfo);
}

TSharedPtr<SWidget> FVoxelSculptToolkit::GetInlineContent() const
{
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.MinDesiredHeight(100.f)
			.MaxDesiredHeight(300.f)
			[
				SNew(SVoxelGraphSelector)
				.IsGraphValid_Lambda([](const UVoxelGraph* Graph)
				{
					if (!ensure(Graph))
					{
						return false;
					}

					return Graph->GetRuntimeGraph().HasNode<FVoxelEditSculptSurfaceExecNode>();
				})
				.OnGraphSelected_Lambda(MakeWeakPtrLambda(this, [this](UVoxelGraph* Graph)
				{
					AVoxelPreviewActor* PreviewActor = WeakPreviewActor.Get();
					if (!PreviewActor)
					{
						return;
					}

					PreviewActor->SetGraph(Graph);
					DetailsView->ForceRefresh();
					ModeDetailsView->ForceRefresh();
				}))
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			DetailsView.ToSharedRef()
		];
}

void FVoxelSculptToolkit::OnToolStarted(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
	FModeToolkit::OnToolStarted(Manager, Tool);

	if (const UVoxelSculptTool* VoxelSculptTool = Cast<UVoxelSculptTool>(Tool))
	{
		WeakPreviewActor = VoxelSculptTool->PreviewActor;
	}
}

void FVoxelSculptToolkit::OnToolEnded(UInteractiveToolManager* Manager, UInteractiveTool* Tool)
{
	FModeToolkit::OnToolEnded(Manager, Tool);

	WeakPreviewActor = nullptr;
}