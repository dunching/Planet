// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Toolkits/VoxelEditorToolkitImpl.h"
#include "Toolkits/ToolkitManager.h"
#include "WorkflowOrientedApp/SModeWidget.h"

TMap<UClass*, UScriptStruct*> GVoxelToolkits;

VOXEL_RUN_ON_STARTUP_EDITOR(RegisterToolkits)
{
	for (UScriptStruct* Struct : GetDerivedStructs<FVoxelToolkit>())
	{
		if (Struct->HasMetaData(STATIC_FNAME("Internal")))
		{
			continue;
		}

		TVoxelInstancedStruct<FVoxelToolkit> Toolkit(Struct);

		const FObjectProperty* Property = Toolkit->GetObjectProperty();
		if (!Property)
		{
			continue;
		}

		ensure(!GVoxelToolkits.Contains(Property->PropertyClass));
		GVoxelToolkits.Add(Property->PropertyClass, Struct);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelToolkitApplicationMode::FVoxelToolkitApplicationMode(const FVoxelToolkit::FMode& Mode)
	: FApplicationMode(Mode.Struct->GetFName())
	, Mode(Mode)
{
}

void FVoxelToolkitApplicationMode::PostActivateMode()
{
	if (ensure(Toolkit))
	{
		Toolkit->LoadDocuments();
	}
}

void FVoxelToolkitApplicationMode::PreDeactivateMode()
{
	if (!ensure(Toolkit))
	{
		return;
	}

	Toolkit->SaveDocuments();
	Toolkit.Reset();

	if (ensure(ToolbarExtension))
	{
		ToolbarExtender->RemoveExtension(ToolbarExtension.ToSharedRef());
		ToolbarExtension.Reset();
	}

	if (ensure(MenuExtension))
	{
		MenuExtender->RemoveExtension(MenuExtension.ToSharedRef());
		MenuExtension.Reset();
	}
}

void FVoxelToolkitApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	ensure(!Toolkit);
	Toolkit = MakeSharedStruct<FVoxelToolkit>(Mode.Struct);

	if (Mode.ConfigureToolkit)
	{
		Mode.ConfigureToolkit(*Toolkit);
	}

	Toolkit->InitializeInternal(Commands, Mode.Object.Get());

	TabLayout = Toolkit->GetLayout();

	ensure(!ToolbarExtension);
	ToolbarExtension = ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		Commands,
		FToolBarExtensionDelegate::CreateSP(Toolkit.ToSharedRef(), &FVoxelToolkit::BuildToolbar));

	ensure(!MenuExtension);
	MenuExtension = MenuExtender->AddMenuBarExtension(
		"Edit",
		EExtensionHook::After,
		Commands,
		FMenuBarExtensionDelegate::CreateSP(Toolkit.ToSharedRef(), &FVoxelToolkit::BuildMenu));

	Toolkit->SetTabManager(InTabManager.ToSharedRef());

	Toolkit->RegisterTabs([&](FName TabId, FText DisplayName, FName IconName, TSharedPtr<SWidget> Widget)
	{
		const FOnSpawnTab OnSpawnTab = FOnSpawnTab::CreateLambda([TabId, DisplayName, IconName, WeakWidget = MakeWeakPtr(Widget)](const FSpawnTabArgs& Args)
		{
			check(Args.GetTabId() == TabId);

			return SNew(SDockTab)
				.Label(DisplayName)
				[
					WeakWidget.IsValid() ? WeakWidget.Pin().ToSharedRef() : SNullWidget::NullWidget
				];
		});

		InTabManager->RegisterTabSpawner(TabId, OnSpawnTab)
			.SetDisplayName(DisplayName)
			.SetGroup(WorkspaceMenuCategory.ToSharedRef())
			.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), IconName));
	});
}

void FVoxelToolkitApplicationMode::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (Toolkit)
	{
		Toolkit->AddReferencedObjects(Collector);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelEditorToolkitImpl::InitVoxelEditor(const TSharedPtr<IToolkitHost>& EditWithinLevelEditor, UObject* ObjectToEdit)
{
	Asset = ObjectToEdit;

	Toolkit = MakeSharedStruct<FVoxelToolkit>(ToolkitStruct);
	Toolkit->InitializeInternal(ToolkitCommands, ObjectToEdit);

	const TArray<FVoxelToolkit::FMode> Modes = Toolkit->GetModes();
	for (const FVoxelToolkit::FMode& Mode : Modes)
	{
		if (ensure(Mode.Object.Get()))
		{
			continue;
		}

		InitAssetEditor(
			EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone,
			EditWithinLevelEditor,
			* (ToolkitStruct->GetName() + TEXT("App")),
			FTabManager::NewLayout("DummyLayout")
			->AddArea
			(
				FTabManager::NewPrimaryArea()
			),
			true,
			true,
			ObjectToEdit,
			false);
		return;
	}

	// Prevent crash on recompile
	if (UBlueprint* Blueprint = Cast<UBlueprint>(ObjectToEdit->GetClass()->ClassGeneratedBy))
	{
		Blueprint->OnCompiled().Add(MakeWeakPtrDelegate(this, [this](UBlueprint*)
		{
			FToolkitManager::Get().CloseToolkit(AsShared());
		}));
	}

	ObjectToEdit->SetFlags(RF_Transactional);

	TSharedPtr<FTabManager::FLayout> StandaloneDefaultLayout;
	if (Modes.Num() == 0)
	{
		StandaloneDefaultLayout = Toolkit->GetLayout();
	}
	else
	{
		// Scratch the toolkit
		Toolkit.Reset();

		StandaloneDefaultLayout =
			FTabManager::NewLayout("DummyLayout")
			->AddArea
			(
				FTabManager::NewPrimaryArea()
			);
	}

	InitAssetEditor(
		EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone,
		EditWithinLevelEditor,
		*(ToolkitStruct->GetName() + TEXT("App")),
		StandaloneDefaultLayout.ToSharedRef(),
		true,
		true,
		ObjectToEdit,
		false);

	if (Modes.Num() > 0)
	{
		FString LastModeName;
		GConfig->GetString(
			TEXT("FVoxelEditorToolkitImpl_LastMode"),
			*ObjectToEdit->GetPathName(),
			LastModeName,
			GEditorPerProjectIni);

		const FName LastModeFName(LastModeName);

		for (const FVoxelToolkit::FMode& Mode : Modes)
		{
			const TSharedRef<FVoxelToolkitApplicationMode> ApplicationMode = MakeVoxelShared<FVoxelToolkitApplicationMode>(Mode);
			AddMenuExtender(ApplicationMode->MenuExtender);
			AddApplicationMode(Mode.Struct->GetFName(), ApplicationMode);
		}

		for (const FVoxelToolkit::FMode& Mode : Modes)
		{
			if (Mode.Struct->GetFName() == LastModeFName &&
				Mode.CanBeSelected.Get())
			{
				SetCurrentMode(LastModeFName);
			}
		}

		if (!GetCurrentModePtr())
		{
			SetCurrentMode(Modes[0].Struct->GetFName());
		}

		if (Modes.Num() > 1)
		{
			const TSharedRef<FExtender> ToolbarExtender = MakeVoxelShared<FExtender>();

			ToolbarExtender->AddToolBarExtension(
				"Asset",
				EExtensionHook::After,
				GetToolkitCommands(),
				MakeWeakPtrDelegate(this, [=](FToolBarBuilder& ToolbarBuilder)
				{
					AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

					for (int32 Index = 0; Index < Modes.Num(); Index++)
					{
						const FVoxelToolkit::FMode& Mode = Modes[Index];

						if (Index != 0)
						{
							AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));
						}

						AddToolbarWidget(
							SNew(SModeWidget, Mode.DisplayName, Mode.Struct->GetFName())
							.OnGetActiveMode(this, &FVoxelEditorToolkitImpl::GetCurrentMode)
							.OnSetActiveMode(FOnModeChangeRequested::CreateSP(this, &FVoxelEditorToolkitImpl::SetCurrentMode))
							.CanBeSelected(Mode.CanBeSelected)
							.ToolTipText(FText::FromString("Switch to " + Mode.DisplayName.ToString() + " mode"))
							.IconImage(Mode.Icon)
						);
					}

					AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));
				}));

			AddToolbarExtender(ToolbarExtender);
		}
	}

	if (Toolkit)
	{
		const TSharedRef<FExtender> MenuExtender = MakeVoxelShared<FExtender>();
		const TSharedRef<FExtender> ToolbarExtender = MakeVoxelShared<FExtender>();

		MenuExtender->AddMenuBarExtension(
			"Edit",
			EExtensionHook::After,
			GetToolkitCommands(),
			FMenuBarExtensionDelegate::CreateSP(Toolkit.ToSharedRef(), &FVoxelToolkit::BuildMenu));

		ToolbarExtender->AddToolBarExtension(
			"Asset",
			EExtensionHook::After,
			GetToolkitCommands(),
			FToolBarExtensionDelegate::CreateSP(Toolkit.ToSharedRef(), &FVoxelToolkit::BuildToolbar));

		AddMenuExtender(MenuExtender);
		AddToolbarExtender(ToolbarExtender);

		Toolkit->LoadDocuments();
	}

	RegenerateMenusAndToolbars();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelEditorToolkitImpl::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);

	ensure(TabManager == InTabManager);
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(GetBaseToolkitName());

	if (Toolkit)
	{
		Toolkit->SetTabManager(InTabManager);
		Toolkit->RegisterTabs([&](FName TabId, FText DisplayName, FName IconName, TSharedPtr<SWidget> Widget)
		{
			const FOnSpawnTab OnSpawnTab = FOnSpawnTab::CreateLambda([TabId, DisplayName, IconName, WeakWidget = MakeWeakPtr(Widget)](const FSpawnTabArgs& Args)
			{
				check(Args.GetTabId() == TabId);

				return SNew(SDockTab)
					.Label(DisplayName)
					[
						WeakWidget.IsValid() ? WeakWidget.Pin().ToSharedRef() : SNullWidget::NullWidget
					];
			});

			InTabManager->RegisterTabSpawner(TabId, OnSpawnTab)
				.SetDisplayName(DisplayName)
				.SetGroup(WorkspaceMenuCategory.ToSharedRef())
				.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), IconName));

			ensure(!RegisteredTabIds.Contains(TabId));
			RegisteredTabIds.Add(TabId);
		});
	}
}

void FVoxelEditorToolkitImpl::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::UnregisterTabSpawners(InTabManager);

	for (const FName& TabId : RegisteredTabIds)
	{
		InTabManager->UnregisterTabSpawner(TabId);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelEditorToolkitImpl::PostRegenerateMenusAndToolbars()
{
	TSharedPtr<FVoxelToolkit> ActiveToolkit;
	if (Toolkit)
	{
		ActiveToolkit = Toolkit;
	}
	else
	{
		const TSharedPtr<FApplicationMode> Mode = GetCurrentModePtr();
		if (!ensure(Mode))
		{
			return;
		}

		ActiveToolkit = StaticCastSharedPtr<FVoxelToolkitApplicationMode>(Mode)->GetToolkit();
	}

	if (!ensure(ActiveToolkit))
	{
		return;
	}

	const TSharedPtr<SWidget> MenuOverlay = ActiveToolkit->GetMenuOverlay();
	if (!MenuOverlay)
	{
		return;
	}

	SetMenuOverlay(MenuOverlay.ToSharedRef());
}

bool FVoxelEditorToolkitImpl::ProcessCommandBindings(const FKeyEvent& InKeyEvent) const
{
	if (FWorkflowCentricApplication::ProcessCommandBindings(InKeyEvent))
	{
		return true;
	}

	const TSharedPtr<FVoxelToolkitApplicationMode> Mode = StaticCastSharedPtr<FVoxelToolkitApplicationMode>(GetCurrentModePtr());
	if (Mode &&
		Mode->Commands->ProcessCommandBindings(InKeyEvent))
	{
		return true;
	}

	return false;
}

void FVoxelEditorToolkitImpl::OnClose()
{
	// OnClose can be called multiple times on shutdown
	if (bClosed)
	{
		return;
	}
	bClosed = true;

	const FName CurrentMode = GetCurrentMode();
	if (!CurrentMode.IsNone() &&
		ensure(Asset.IsValid()))
	{
		GConfig->SetString(
			TEXT("FVoxelEditorToolkitImpl_LastMode"),
			*Asset->GetPathName(),
			*CurrentMode.ToString(),
			GEditorPerProjectIni);
	}

	if (Toolkit)
	{
		Toolkit->SaveDocuments();
	}

	FWorkflowCentricApplication::OnClose();
}

void FVoxelEditorToolkitImpl::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (Toolkit)
	{
		Toolkit->AddStructReferencedObjects(Collector);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UScriptStruct* FVoxelEditorToolkitImpl::GetToolkitStruct(const UClass* Class)
{
	while (Class)
	{
		if (UScriptStruct* Struct = GVoxelToolkits.FindRef(Class))
		{
			return Struct;
		}

		Class = Class->GetSuperClass();
	}

	return nullptr;
}

TSharedPtr<FVoxelEditorToolkitImpl> FVoxelEditorToolkitImpl::MakeToolkit(const UClass* Class)
{
	UScriptStruct* Struct = GetToolkitStruct(Class);
	if (!Struct)
	{
		return nullptr;
	}

	return MakeVoxelShared<FVoxelEditorToolkitImpl>(Struct);
}