// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Widgets/SVoxelChannelEditor.h"

#include "VoxelChannel.h"
#include "VoxelSettings.h"
#include "VoxelGraphVisuals.h"
#include "Customizations/VoxelChannelEditorDefinitionCustomization.h"

#include "IStructureDetailsView.h"
#include "SVoxelReadWriteFilePermissions.h"

void SVoxelChannelEditor::Construct(const FArguments& InArgs)
{
	MaxHeight = InArgs._MaxHeight;
	SelectedChannel = InArgs._SelectedChannel;
	OnChannelSelectedDelegate = InArgs._OnChannelSelected;

	MapChannels();

	ChildSlot
	[
		SNew(SBorder)
		.Padding(2.f)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Top)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Lambda([this]
					{
						for (const auto& It : AllChannels)
						{
							ChannelsTreeWidget->SetItemExpansion(It.Value, true);
						}

						return FReply::Handled();
					})
					.Text(INVTEXT("Expand"))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Lambda([this]
					{
						ChannelsTreeWidget->ClearExpandedItems();
						return FReply::Handled();
					})
					.Text(INVTEXT("Collapse"))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
				.Padding(5.f, 1.f)
				[
					SNew(SSearchBox)
					.HintText(INVTEXT("Search for Channel"))
					.OnTextChanged_Lambda([this](const FText& NewText)
					{
						LookupString = NewText;
						FilterChannels(false);
					})
				]
			]
			+ SVerticalBox::Slot()
			.MaxHeight(MaxHeight)
			[
				SAssignNew(ChannelsTreeBox, SBox)
				[
					SAssignNew(ChannelsTreeWidget, SChannelsTree)
					.TreeItemsSource(&ChannelItems)
					.OnGenerateRow(this, &SVoxelChannelEditor::OnGenerateRow)
					.OnGetChildren(this, &SVoxelChannelEditor::OnGetChildren)
					.OnSelectionChanged(this, &SVoxelChannelEditor::OnSelectionChanged)
					.SelectionMode(ESelectionMode::Single)
				]
			]
		]
	];

	if (const TSharedPtr<FChannelNode> ChannelNode = AllChannels.FindRef(SelectedChannel))
	{
		ChannelsTreeWidget->SetSelection(ChannelNode);
	}

	FilterChannels(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVector2D SVoxelChannelEditor::ComputeDesiredSize(const float LayoutScaleMultiplier) const
{
	FVector2D WidgetSize = SCompoundWidget::ComputeDesiredSize(LayoutScaleMultiplier);

	const FVector2D TagTreeContainerSize = ChannelsTreeBox->GetDesiredSize();
	if (TagTreeContainerSize.Y < MaxHeight)
	{
		WidgetSize.Y += MaxHeight - TagTreeContainerSize.Y;
	}

	return WidgetSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<ITableRow> SVoxelChannelEditor::OnGenerateRow(TSharedPtr<FChannelNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedPtr<SWidget> TypeBox = SNullWidget::NullWidget;

	if (!InItem->bIsCategory)
	{
		TOptional<FVoxelChannelDefinition> ChannelDef = GVoxelChannelManager->FindChannelDefinition(InItem->FullPath);
		if (ChannelDef.IsSet())
		{
			TypeBox =
				SNew(SImage)
				.Image(FVoxelGraphVisuals::GetPinIcon(ChannelDef->Type).GetIcon())
				.ColorAndOpacity(FVoxelGraphVisuals::GetPinColor(ChannelDef->Type))
				.DesiredSizeOverride(FVector2D(14.f, 14.f));
		}
	}

	const TSharedRef<SHorizontalBox> HorizontalBox =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .Padding(1.f)
		  .VAlign(VAlign_Center)
		[
			TypeBox.ToSharedRef()
		]
		+ SHorizontalBox::Slot()
		  .FillWidth(1.f)
		  .HAlign(HAlign_Left)
		  .VAlign(VAlign_Center)
		[
			SNew(SVoxelDetailText)
			.Font(FAppStyle::Get().GetFontStyle(InItem->bIsCategory ? "DetailsView.CategoryFontStyle" : "PropertyWindow.NormalFont"))
			.ColorAndOpacity(FLinearColor::White)
			.Text(FText::FromName(InItem->Name))
			.HighlightText_Lambda([this]
			{
				return LookupString;
			})
		]
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .HAlign(HAlign_Right)
		[
			SNew(SButton)
			.ToolTipText(INVTEXT("Add sub channel"))
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.DesiredSizeScale(0.9f)
			.ContentPadding(2.f)
			.ForegroundColor(FSlateColor::UseForeground())
			.IsFocusable(false)
			.OnClicked(this, &SVoxelChannelEditor::OnAddSubChannelClicked, InItem)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.PlusCircle"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		];

	if (InItem->bIsCategory)
	{
		if (!InItem->Parent.IsValid() &&
			Cast<UVoxelChannelRegistry>(InItem->Owner.Get()))
		{
			HorizontalBox->AddSlot()
			  .AutoWidth()
			  .HAlign(HAlign_Right)
			[
				SNew(SButton)
				.ToolTipText(INVTEXT("Browse to channel"))
				.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
				.DesiredSizeScale(0.9f)
				.ContentPadding(2.f)
				.ForegroundColor(FSlateColor::UseForeground())
				.IsFocusable(false)
				.OnClicked(this, &SVoxelChannelEditor::OnBrowseToChannel, InItem)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.BrowseContent"))
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			];
		}
	}
	else
	{
		HorizontalBox->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		[
			SNew(SButton)
			.ToolTipText(INVTEXT("Edit channel"))
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.DesiredSizeScale(0.9f)
			.ContentPadding(2.f)
			.ForegroundColor(FSlateColor::UseForeground())
			.IsFocusable(false)
			.OnClicked(this, &SVoxelChannelEditor::OnEditChannelClicked, InItem)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Edit"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		];

		HorizontalBox->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		[
			SNew(SButton)
			.ToolTipText(INVTEXT("Delete channel"))
			.ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
			.DesiredSizeScale(0.9f)
			.ContentPadding(2.f)
			.ForegroundColor(FSlateColor::UseForeground())
			.IsFocusable(false)
			.OnClicked(this, &SVoxelChannelEditor::OnDeleteChannelClicked, InItem)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Icons.Delete"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		];
	}

	return
		SNew(STableRow<TSharedPtr<FChannelNode>>, OwnerTable)
		.Style(FAppStyle::Get(), "GameplayTagTreeView")
		[
			HorizontalBox
		];
}

void SVoxelChannelEditor::OnGetChildren(const TSharedPtr<FChannelNode> Item, TArray<TSharedPtr<FChannelNode>>& OutChildren) const
{
	for (const auto& It : Item->Channels)
	{
		OutChildren.Add(It.Value);
	}

	OutChildren.Sort([](const TSharedPtr<FChannelNode>& A, const TSharedPtr<FChannelNode>& B)
	{
		if (A->bIsCategory == B->bIsCategory)
		{
			return A->Name.ToString() < B->Name.ToString();
		}

		return A->bIsCategory;
	});
}

void SVoxelChannelEditor::OnSelectionChanged(TSharedPtr<FChannelNode> ChannelNode, const ESelectInfo::Type SelectType)
{
	if (SelectType != ESelectInfo::OnMouseClick ||
		!ChannelNode)
	{
		return;
	}

	if (ChannelNode->bIsCategory)
	{
		if (const TSharedPtr<FChannelNode> SelectedChannelNode = AllChannels.FindRef(SelectedChannel))
		{
			ChannelsTreeWidget->SetSelection(SelectedChannelNode);
		}

		return;
	}

	OnChannelSelectedDelegate.ExecuteIfBound(ChannelNode->FullPath);
}

FReply SVoxelChannelEditor::OnAddSubChannelClicked(TSharedPtr<FChannelNode> Item)
{
	AddEditButtonText = INVTEXT("Add new Channel");

	FName FullPath = FName((Item ? Item->FullPath.ToString() : "Project") + ".MyChannel");
	while (AllChannels.Contains(FullPath))
	{
		FullPath.SetNumber(FullPath.GetNumber() + 1);
	}

	FVoxelChannelEditorDefinition Def;
	Def.bEditChannel = false;
	Def.SaveLocation = Item ? Item->Owner : GetMutableDefault<UVoxelSettings>();
	Def.Type = FVoxelPinType::Make<float>().GetBufferType();
	Def.DefaultValue = FVoxelPinValue(Def.Type.GetExposedType());
	Def.Name = FName(FullPath.ToString().RightChop(FullPath.ToString().Find(".") + 1));

	TSet<FName> ChannelsList;
	AllChannels.GetKeys(ChannelsList);
	SVoxelChannelEditDialog::EditChannel(ChannelsList, Def, OnChannelSelectedDelegate);

	return FReply::Handled();
}

FReply SVoxelChannelEditor::OnEditChannelClicked(TSharedPtr<FChannelNode> Item)
{
	AddEditButtonText = INVTEXT("Edit Channel");

	FVoxelChannelEditorDefinition ChannelToEdit;
	ChannelToEdit.bEditChannel = true;
	ChannelToEdit.SaveLocation = Item->Owner;
	ChannelToEdit.Name = FName(Item->FullPath.ToString().RightChop(Item->FullPath.ToString().Find(".") + 1));

	TOptional<FVoxelChannelDefinition> ChannelDef = GVoxelChannelManager->FindChannelDefinition(Item->FullPath);
	if (ensure(ChannelDef.IsSet()))
	{
		ChannelToEdit.Type = ChannelDef->Type;
		ChannelToEdit.DefaultValue = ChannelDef->GetExposedDefaultValue();
	}
	else
	{
		ChannelToEdit.Type = FVoxelPinType::Make<float>().GetBufferType();
		ChannelToEdit.DefaultValue = FVoxelPinValue(ChannelToEdit.Type.GetExposedType());
	}

	TSet<FName> ChannelsList;
	AllChannels.GetKeys(ChannelsList);

	SVoxelChannelEditDialog::EditChannel(ChannelsList, ChannelToEdit, OnChannelSelectedDelegate);

	return FReply::Handled();
}

FReply SVoxelChannelEditor::OnDeleteChannelClicked(TSharedPtr<FChannelNode> Item)
{
	UObject* SaveLocation = Item->Owner.Get();
	if (!SaveLocation)
	{
		return FReply::Handled();
	}

	const FName FullPath = FName(Item->FullPath.ToString().RightChop(Item->FullPath.ToString().Find(".") + 1));
	if (!SVoxelReadWriteFilePermissionsPopup::PromptForPermissions(GetChannelFilePath(SaveLocation)))
	{
		return FReply::Handled();
	}

	if (UVoxelSettings* Settings = Cast<UVoxelSettings>(SaveLocation))
	{
		FVoxelTransaction Transaction(Settings, "Delete Channel");
		Transaction.SetProperty(FindFPropertyChecked(UVoxelSettings, GlobalChannels));

		for (auto It = Settings->GlobalChannels.CreateIterator(); It; ++It)
		{
			if (It->Name == FullPath)
			{
				It.RemoveCurrentSwap();
				break;
			}
		}

		Settings->TryUpdateDefaultConfigFile();

		// Force save config now
		GConfig->Flush(false, GEngineIni);
	}
	else if (UVoxelChannelRegistry* Registry = Cast<UVoxelChannelRegistry>(SaveLocation))
	{
		FVoxelTransaction Transaction(Registry, "Delete Channel");
		Transaction.SetProperty(FindFPropertyChecked(UVoxelChannelRegistry, Channels));

		for (auto It = Registry->Channels.CreateIterator(); It; ++It)
		{
			if (It->Name == FullPath)
			{
				It.RemoveCurrentSwap();
				break;
			}
		}
	}
	else
	{
		ensure(false);
		return FReply::Handled();
	}

	if (SelectedChannel == Item->FullPath)
	{
		OnChannelSelectedDelegate.ExecuteIfBound(STATIC_FNAME("Project.Surface"));
	}

	return FReply::Handled();
}

FReply SVoxelChannelEditor::OnBrowseToChannel(TSharedPtr<FChannelNode> Item) const
{
	UObject* Object = Item->Owner.Get();
	if (!ensure(Cast<UVoxelChannelRegistry>(Object)))
	{
		return FReply::Handled();
	}

	FSlateApplication::Get().DismissAllMenus();

	TArray<UObject*> Assets{ Object };
	GEditor->SyncBrowserToObjects(Assets, true);

	return FReply::Handled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelChannelEditor::MapChannels()
{
	AllChannels = {};

	TSet<const UObject*> ExistingAssets;
	for (const UObject* Asset : GVoxelChannelManager->GetChannelAssets())
	{
		ExistingAssets.Add(Asset);

		FString Prefix;
		const TArray<FVoxelChannelExposedDefinition>* ChannelsList;
		if (const UVoxelSettings* Settings = Cast<UVoxelSettings>(Asset))
		{
			ChannelsList = &Settings->GlobalChannels;
			Prefix = "Project.";
		}
		else if (const UVoxelChannelRegistry* Registry = Cast<UVoxelChannelRegistry>(Asset))
		{
			ChannelsList = &Registry->Channels;
			Prefix = Registry->GetName() + ".";
		}
		else
		{
			ensure(false);
			continue;
		}

		if (ChannelsList->IsEmpty())
		{
			Prefix.RemoveFromEnd(".");
			FName Name = *Prefix;

			TSharedRef<FChannelNode> Node = MakeShared<FChannelNode>();
			Node->Name = Name;
			Node->FullPath = Name;
			Node->Owner = ConstCast(Asset);

			AllChannels.Add(Name, Node);
			continue;
		}

		for (const FVoxelChannelExposedDefinition& Channel : *ChannelsList)
		{
			FString ChannelPath = Prefix + Channel.Name.ToString();

			TArray<FString> Path;
			ChannelPath.ParseIntoArray(Path, TEXT("."));

			if (!ensure(Path.Num() > 0))
			{
				continue;
			}

			FName RootPath = FName(Path[0]);
			TSharedPtr<FChannelNode> Node = AllChannels.FindRef(RootPath);
			if (!Node)
			{
				Node = MakeShared<FChannelNode>();
				Node->Name = RootPath;
				Node->FullPath = RootPath;
				Node->Owner = ConstCast(Asset);

				AllChannels.Add(RootPath, Node);
			}

			FString FullPath = RootPath.ToString() + ".";
			for (int32 Index = 1; Index < Path.Num(); Index++)
			{
				FullPath += Path[Index];

				FName CategoryPath = FName((Index < Path.Num() - 1 ? "CAT|" : "") + FullPath);
				TSharedPtr<FChannelNode> SubNode = AllChannels.FindRef(CategoryPath);
				if (!SubNode)
				{
					SubNode = MakeShared<FChannelNode>();
					SubNode->Name = FName(Path[Index]);
					SubNode->FullPath = CategoryPath;
					SubNode->Parent = Node;
					SubNode->Owner = ConstCast(Asset);

					AllChannels.Add(CategoryPath, SubNode);
				}

				Node = SubNode;

				FullPath += ".";
			}

			Node->FullPath = FName(ChannelPath);
			Node->bIsCategory = false;
		}
	}
}

void SVoxelChannelEditor::FilterChannels(const bool bOnlySelectedChannel)
{
	ChannelItems = {};

	const FString SearchString = LookupString.ToString();

	TArray<TSharedPtr<FChannelNode>> FilteredChannels;
	for (const auto& It : AllChannels)
	{
		It.Value->Channels = {};

		if (It.Value->Name.ToString().Contains(SearchString))
		{
			FilteredChannels.Add(It.Value);
		}
	}

	ChannelsTreeWidget->ClearExpandedItems();

	TSet<FName> AddedRootNodes;
	for (const TSharedPtr<FChannelNode>& ChannelNode : FilteredChannels)
	{
		TSharedPtr<FChannelNode> Node = ChannelNode;
		while (Node)
		{
			if (!bOnlySelectedChannel ||
				ChannelNode->FullPath == SelectedChannel)
			{
				ChannelsTreeWidget->SetItemExpansion(Node, true);
			}

			const TSharedPtr<FChannelNode> Parent = Node->Parent.Pin();
			if (!Parent)
			{
				if (!AddedRootNodes.Contains(Node->Name))
				{
					AddedRootNodes.Add(Node->Name);
					ChannelItems.Add(Node);
				}
				break;
			}

			Parent->Channels.Add(Node->FullPath, Node);
			Node = Parent;
		}
	}

	ChannelsTreeWidget->RequestTreeRefresh();
}

FString SVoxelChannelEditor::GetChannelFilePath(UObject* Object)
{
	if (const UVoxelSettings* Settings = Cast<UVoxelSettings>(Object))
	{
		return Settings->GetDefaultConfigFilename();
	}

	if (const UVoxelChannelRegistry* Registry = Cast<UVoxelChannelRegistry>(Object))
	{
		FString FilePath;
		ensure(FPackageName::TryConvertLongPackageNameToFilename(Registry->GetPackage()->GetName(), FilePath, FPackageName::GetAssetPackageExtension()));
		return FilePath;
	}

	ensure(false);
	return {};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelChannelEditDialog::Construct(const FArguments& InArgs)
{
	WeakParentWindow = InArgs._ParentWindow;
	ChannelsList = InArgs._ChannelsList;
	ChannelToEdit = InArgs._ChannelToEdit;
	OnChannelSelected = InArgs._OnChannelSelected;

	CreateDetailsView();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
		[
			SNew(SBox)
			.WidthOverride(450.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(SettingsCheckoutNotice, SVoxelReadWriteFilePermissionsNotice)
					.FilePath(SVoxelChannelEditor::GetChannelFilePath(ChannelToEdit.SaveLocation.Get()))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					DetailsView->GetWidget().ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				.Padding(2.f)
				[
					SNew(SButton)
					.Text(INVTEXT("Confirm"))
					.OnClicked(this, &SVoxelChannelEditDialog::OnCreateEditChannelClicked)
				]
			]
		]
	];
}

void SVoxelChannelEditDialog::EditChannel(const TSet<FName>& ChannelsList, const FVoxelChannelEditorDefinition& Channel, const SVoxelChannelEditor::FOnChannelSelected& Delegate)
{
	const TSharedRef<SWindow> EditWindow = SNew(SWindow)
		.Title(Channel.bEditChannel ? INVTEXT("Edit Channel") : INVTEXT("Create Channel"))
		.SizingRule(ESizingRule::Autosized)
		.ClientSize(FVector2D(0.f, 300.f))
		.SupportsMaximize(false)
		.SupportsMinimize(false);

	const TSharedRef<SVoxelChannelEditDialog> ChannelEditDialog = SNew(SVoxelChannelEditDialog)
		.ParentWindow(EditWindow)
		.ChannelsList(ChannelsList)
		.ChannelToEdit(Channel)
		.OnChannelSelected(Delegate);

	EditWindow->SetContent(ChannelEditDialog);

	GEditor->EditorAddModalWindow(EditWindow);
}

FReply SVoxelChannelEditDialog::OnCreateEditChannelClicked()
{
	ON_SCOPE_EXIT
	{
		if (const TSharedPtr<SWindow> PinnedWindow = WeakParentWindow.Pin())
		{
			PinnedWindow->RequestDestroyWindow();
		}
	};

	const FVoxelChannelEditorDefinition* ChannelDef = StructOnScope->Get();
	UObject* SaveLocation = ChannelDef->SaveLocation.Get();
	if (!SaveLocation)
	{
		return FReply::Handled();
	}

	if (ensure(SettingsCheckoutNotice) &&
		!SettingsCheckoutNotice->IsUnlocked())
	{
		const EAppReturnType::Type DialogResult = FMessageDialog::Open(
			EAppMsgType::YesNo,
			EAppReturnType::No,
			INVTEXT("You're about to make changes to a file which isn't writable. Whatever changes you make will not be saved when the editor is closed. Continue anyway?"),
			ChannelDef->bEditChannel ? INVTEXT("Edit Channel") : INVTEXT("Create new Channel"));

		if (DialogResult == EAppReturnType::No)
		{
			return FReply::Handled();
		}
	}

	FName NewChannelPath;
	const FVoxelChannelExposedDefinition NewChannel = *ChannelDef;
	if (UVoxelSettings* Settings = Cast<UVoxelSettings>(SaveLocation))
	{
		{
			FVoxelTransaction Transaction(Settings, "Add new Channel");
			Transaction.SetProperty(FindFPropertyChecked(UVoxelSettings, GlobalChannels));

			if (ChannelDef->bEditChannel)
			{
				for (FVoxelChannelExposedDefinition& Channel : Settings->GlobalChannels)
				{
					if (Channel.Name != ChannelDef->Name)
					{
						continue;
					}

					Channel.Type = ChannelDef->Type;
					Channel.DefaultValue = ChannelDef->DefaultValue;
					NewChannelPath = "Project." + Channel.Name;
					break;
				}
			}
			else
			{
				Settings->GlobalChannels.Add(NewChannel);
			}

			Settings->TryUpdateDefaultConfigFile();

			// Force save config now
			GConfig->Flush(false, GEngineIni);
		}

		if (!ChannelDef->bEditChannel &&
			ensure(Settings->GlobalChannels.Num() > 0))
		{
			NewChannelPath = "Project." + Settings->GlobalChannels[Settings->GlobalChannels.Num() - 1].Name;
		}
	}
	else if (UVoxelChannelRegistry* Registry = Cast<UVoxelChannelRegistry>(SaveLocation))
	{
		{
			FVoxelTransaction Transaction(Registry, "Add new Channel");
			Transaction.SetProperty(FindFPropertyChecked(UVoxelChannelRegistry, Channels));

			if (ChannelDef->bEditChannel)
			{
				for (FVoxelChannelExposedDefinition& Channel : Registry->Channels)
				{
					if (Channel.Name != ChannelDef->Name)
					{
						continue;
					}

					Channel.Type = ChannelDef->Type;
					Channel.DefaultValue = ChannelDef->DefaultValue;
					NewChannelPath = Registry->GetName() + "." + Channel.Name;
					break;
				}
			}
			else
			{
				Registry->Channels.Add(NewChannel);
			}
		}

		if (!ChannelDef->bEditChannel &&
			ensure(Registry->Channels.Num() > 0))
		{
			NewChannelPath = Registry->GetName() + "." + Registry->Channels[Registry->Channels.Num() - 1].Name;
		}
	}
	else
	{
		ensure(false);
		return FReply::Handled();
	}

	OnChannelSelected.ExecuteIfBound(NewChannelPath);

	return FReply::Handled();
}

void SVoxelChannelEditDialog::CreateDetailsView()
{
	StructOnScope = MakeShared<TStructOnScope<FVoxelChannelEditorDefinition>>();
	StructOnScope->InitializeAs<FVoxelChannelEditorDefinition>(ChannelToEdit);

	FDetailsViewArgs Args;
	Args.bAllowSearch = false;
	Args.bShowOptions = false;
	Args.bHideSelectionTip = true;
	Args.bShowPropertyMatrixButton = false;
	Args.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	const FStructureDetailsViewArgs StructArgs;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsView = PropertyModule.CreateStructureDetailView(Args, StructArgs, StructOnScope);
	DetailsView->GetDetailsView()->SetDisableCustomDetailLayouts(false);
	DetailsView->GetDetailsView()->SetGenericLayoutDetailsDelegate(MakeLambdaDelegate([]() -> TSharedRef<IDetailCustomization>
	{
		return MakeVoxelShared<FVoxelChannelEditorDefinitionCustomization>();
	}));
	DetailsView->GetDetailsView()->ForceRefresh();

	DetailsView->GetDetailsView()->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& PropertyChangedEvent)
	{
		FVoxelChannelEditorDefinition* ChannelDefinition = StructOnScope->Get();
		if (!ensure(ChannelDefinition))
		{
			return;
		}

		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STATIC(FVoxelChannelEditorDefinition, Type))
		{
			ChannelDefinition->Fixup();
			DetailsView->GetDetailsView()->ForceRefresh();
		}
		else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STATIC(FVoxelChannelEditorDefinition, SaveLocation))
		{
			UObject* SaveLocation = ChannelDefinition->SaveLocation.Get();
			if (!ensure(SaveLocation))
			{
				return;
			}

			FString RootName = "Project";
			if (!Cast<UVoxelSettings>(SaveLocation))
			{
				RootName = SaveLocation->GetName();
			}

			FName NewPath = FName(RootName + "." + ChannelDefinition->Name);
			while (ChannelsList.Contains(NewPath))
			{
				NewPath.SetNumber(NewPath.GetNumber() + 1);
			}

			ChannelDefinition->Name = FName(NewPath.ToString().RightChop(NewPath.ToString().Find(".") + 1));

			SettingsCheckoutNotice->SetFilePath(SVoxelChannelEditor::GetChannelFilePath(SaveLocation));
			SettingsCheckoutNotice->Invalidate();
		}
	});
}