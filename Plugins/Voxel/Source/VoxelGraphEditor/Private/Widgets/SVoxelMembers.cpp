// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Widgets/SVoxelMembers.h"

#include "SVoxelGraphSearch.h"
#include "VoxelGraphToolkit.h"
#include "SchemaActions/VoxelMembersBaseSchemaAction.h"
#include "DragDropActions/VoxelMembersCategoryDragDropAction.h"

#if VOXEL_ENGINE_VERSION >= 503
#include "GraphActionNode.h"
#else
#include "GraphEditor/Private/GraphActionNode.h"
#endif

void FVoxelMembersActionsSortHelper::AddCategoriesSortList(const int32 SectionId, const TArray<FString>& Categories)
{
	SortedCategories.Add(SectionId, Categories);
	SortedCategories[SectionId].Add("");
}

void FVoxelMembersActionsSortHelper::AddAction(const TSharedRef<FEdGraphSchemaAction>& NewAction, const FString& Category)
{
	auto& SectionActions = Actions.FindOrAdd(NewAction->GetSectionID(), {});
	auto& CategoryActions = SectionActions.FindOrAdd(Category, {});
	CategoryActions.Add(NewAction);
}

void FVoxelMembersActionsSortHelper::GetAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	for (const auto& It : SortedCategories)
	{
		auto* ActionsMapPtr = Actions.Find(It.Key);
		if (!ActionsMapPtr)
		{
			continue;
		}

		for (const FString& Category : It.Value)
		{
			if (const auto* ActionsPtr = ActionsMapPtr->Find(Category))
			{
				for (const TSharedRef<FEdGraphSchemaAction>& Action : *ActionsPtr)
				{
					OutAllActions.AddAction(Action);
				}
			}
			ActionsMapPtr->Remove(Category);
		}
	}

	for (const auto& It : Actions)
	{
		if (ensure(It.Value.Num() == 0))
		{
			continue;
		}

		for (const auto& InnerIt : It.Value)
		{
			for (auto Action : InnerIt.Value)
			{
				OutAllActions.AddAction(Action);
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::Construct(const FArguments& Args)
{
	WeakObject = Args._Object;
	WeakToolkit = Args._Toolkit;

	check(WeakToolkit.IsValid());

	CommandList = MakeVoxelShared<FUICommandList>();

	CommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SVoxelMembers::OnDeleteEntry));
	CommandList->MapAction(FGenericCommands::Get().Duplicate, FExecuteAction::CreateSP(this, &SVoxelMembers::OnDuplicateAction));

	CommandList->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &SVoxelMembers::OnRequestRenameOnActionNode),
		FCanExecuteAction::CreateSP(this, &SVoxelMembers::CanRequestRenameOnActionNode));

	CommandList->MapAction(FGenericCommands::Get().Copy, FExecuteAction::CreateSP(this, &SVoxelMembers::OnCopy));
	CommandList->MapAction(FGenericCommands::Get().Cut, FExecuteAction::CreateSP(this, &SVoxelMembers::OnCut));

	CommandList->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateSP(this, &SVoxelMembers::OnPaste),
		FCanExecuteAction::CreateSP(this, &SVoxelMembers::CanPaste));
	if (GetToolkit<FVoxelGraphToolkit>())
	{
		CommandList->MapAction(FGraphEditorCommands::Get().FindReferences, FExecuteAction::CreateSP(this, &SVoxelMembers::OnFindReferences));
	}

	SAssignNew(FilterBox, SSearchBox)
	.OnTextChanged(this, &SVoxelMembers::OnFilterTextChanged);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(4.0f)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				FilterBox.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SAssignNew(MembersMenu, SGraphActionMenu, false)
			.AlphaSortItems(false)
			.AutoExpandActionMenu(true)
			.OnCollectStaticSections(this, &SVoxelMembers::CollectStaticSections)
			.OnGetSectionTitle(this, &SVoxelMembers::OnGetSectionTitle)
			.OnGetSectionWidget(this, &SVoxelMembers::OnGetMenuSectionWidget)
			.OnCollectAllActions(this, &SVoxelMembers::CollectAllActions)
			.OnCreateWidgetForAction(this, &SVoxelMembers::OnCreateWidgetForAction)
			.OnCanRenameSelectedAction_Lambda([](TWeakPtr<FGraphActionNode>) { return true; })
			.OnActionSelected(this, &SVoxelMembers::OnActionSelected)
			.OnActionMatchesName(this, &SVoxelMembers::HandleActionMatchesName)
			.OnActionDragged(this, &SVoxelMembers::OnActionDragged)
			.OnCategoryTextCommitted(this, &SVoxelMembers::OnCategoryNameCommitted)
			.OnCategoryDragged(this, &SVoxelMembers::OnCategoryDragged)
			.OnGetFilterText(this, &SVoxelMembers::GetFilterText)
			.OnContextMenuOpening(this, &SVoxelMembers::OnContextMenuOpening)
			.OnActionDoubleClicked(this, &SVoxelMembers::OnMemberActionDoubleClicked)
			.UseSectionStyling(true)
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (bNeedsRefresh)
	{
		Refresh();
	}
}

FReply SVoxelMembers::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (CommandList.IsValid() &&
		CommandList->ProcessCommandBindings(InKeyEvent))
	{
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::RequestRefresh()
{
	bNeedsRefresh = true;
}

void SVoxelMembers::SelectMember(const FName Name, const int32 SectionId, const bool bRequestRename, const bool bRefresh)
{
	if (bRefresh)
	{
		Refresh();
	}

	MembersMenu->SelectItemByName(Name, ESelectInfo::Direct, SectionId);

	if (!bRequestRename)
	{
		return;
	}

	OnRequestRenameOnActionNode();

	TArray<TSharedPtr<FEdGraphSchemaAction>> SelectedActions;
	MembersMenu->GetSelectedActions(SelectedActions);

	OnActionSelected(SelectedActions, ESelectInfo::OnMouseClick);
}

FVoxelMembersColumnSizeData& SVoxelMembers::GetColumnSizeData()
{
	return ColumnSizeData;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::PostUndo(bool bSuccess)
{
	if (!GetObject())
	{
		SelectBaseObject();
	}

	Refresh();
}

void SVoxelMembers::PostRedo(bool bSuccess)
{
	Refresh();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::Refresh()
{
	bNeedsRefresh = false;

	TGuardValue<bool> Guard(bIsRefreshing, true);
	MembersMenu->RefreshAllActions(true);
}

FString SVoxelMembers::GetPasteCategory() const
{
	TArray<TSharedPtr<FEdGraphSchemaAction>> SelectedActions;
	MembersMenu->GetSelectedActions(SelectedActions);

	if (SelectedActions.Num() != 0 ||
		!MembersMenu.IsValid())
	{
		return "";
	}

	return MembersMenu->GetSelectedCategoryName();
}

TSharedRef<SWidget> SVoxelMembers::CreateAddButton(const int32 SectionId, const FText& AddNewText, const FName MetaDataTag)
{
	return
		SNew(SButton)
		.ButtonStyle(FAppStyle::Get(), "SimpleButton")
		.OnClicked_Lambda([this, SectionId]
		{
			OnAddNewMember(SectionId);
			return FReply::Handled();
		})
		.ContentPadding(FMargin(1, 0))
		.AddMetaData<FTagMetaData>(FTagMetaData(MetaDataTag))
		.ToolTipText(AddNewText)
		[
			SNew(SImage)
			.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
			.ColorAndOpacity(FSlateColor::UseForeground())
		];
}

TSharedPtr<FVoxelMembersBaseSchemaAction> SVoxelMembers::GetAction(const TSharedPtr<FEdGraphSchemaAction>& Action) const
{
	if (!ensure(Action))
	{
		return nullptr;
	}

	TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = StaticCastSharedPtr<FVoxelMembersBaseSchemaAction>(Action);
	if (!ensure(BaseAction))
	{
		return nullptr;
	}

	return BaseAction;
}

TSharedPtr<FVoxelMembersBaseSchemaAction> SVoxelMembers::GetAction(const TArray<TSharedPtr<FEdGraphSchemaAction>>& Actions) const
{
	if (Actions.Num() == 0)
	{
		return nullptr;
	}

	return GetAction(Actions[0]);
}

TSharedPtr<FVoxelMembersBaseSchemaAction> SVoxelMembers::GetAction() const
{
	TArray<TSharedPtr<FEdGraphSchemaAction>> SelectedActions;
	MembersMenu->GetSelectedActions(SelectedActions);

	return GetAction(SelectedActions);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelMembersActionsSortHelper ActionsList;
	CollectSortedActions(ActionsList);
	ActionsList.GetAllActions(OutAllActions);
}

TSharedRef<SWidget> SVoxelMembers::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData) const
{
	if (const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction(InCreateData->Action))
	{
		return BaseAction->CreatePaletteWidget(InCreateData);
	}

	return SNullWidget::NullWidget;
}

void SVoxelMembers::OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType)
{
	if (InSelectionType != ESelectInfo::OnMouseClick &&
		InSelectionType != ESelectInfo::OnKeyPress &&
		InSelectionType != ESelectInfo::OnNavigation &&
		InActions.Num() > 0)
	{
		return;
	}

	if (InActions.Num() == 0)
	{
		if (!bIsRefreshing &&
			InSelectionType != ESelectInfo::Direct &&
			MembersMenu->GetSelectedCategoryName().IsEmpty())
		{
			SelectBaseObject();
		}
		return;
	}

	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction(InActions);
	if (ensure(BaseAction))
	{
		BaseAction->OnSelected();
	}
}

bool SVoxelMembers::HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName)
{
	return FName(*InAction->GetMenuDescription().ToString()) == InName;
}

FReply SVoxelMembers::OnActionDragged(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, const FPointerEvent& MouseEvent)
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction(InActions);
	if (!ensure(BaseAction))
	{
		return FReply::Unhandled();
	}

	return BaseAction->OnDragged(WeakObject.Get(), BaseAction, MouseEvent);
}

void SVoxelMembers::OnMemberActionDoubleClicked(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions)
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction(InActions);
	if (!ensure(BaseAction))
	{
		return;
	}

	BaseAction->OnActionDoubleClick();
}

void SVoxelMembers::OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr<FGraphActionNode> InAction)
{
	const TSharedPtr<FVoxelToolkit> Toolkit = GetToolkit();
	if (!ensure(Toolkit))
	{
		return;
	}

	const FString CategoryName = InNewText.ToString().TrimStartAndEnd();

	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
	MembersMenu->GetCategorySubActions(InAction, Actions);

	const TSharedPtr<FGraphActionNode> CategoryAction = InAction.Pin();
	if (Actions.Num() == 0 ||
		!ensure(CategoryAction))
	{
		return;
	}

	{
		const FVoxelTransaction Transaction(Toolkit->GetAsset(), "Rename Category");

		for (int32 Index = 0; Index < Actions.Num(); ++Index)
		{
			if (!Actions[Index])
			{
				continue;
			}

			const TSharedPtr<FVoxelMembersBaseSchemaAction> MemberAction = StaticCastSharedPtr<FVoxelMembersBaseSchemaAction>(Actions[Index]);
			if (!MemberAction)
			{
				continue;
			}

			MemberAction->SetCategory(CategoryName);
		}
	}

	RequestRefresh();
	MembersMenu->SelectItemByName(FName(*CategoryName), ESelectInfo::OnMouseClick, CategoryAction->SectionID, true);
}

FReply SVoxelMembers::OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent)
{
	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
	MembersMenu->GetSelectedCategorySubActions(Actions);

	if (Actions.Num() == 0)
	{
		return FReply::Handled();
	}

	if (!ensure(WeakObject.IsValid()))
	{
		return FReply::Handled();
	}

	return FReply::Handled().BeginDragDrop(FVoxelMembersCategoryDragDropAction::New(InCategory.ToString(), SharedThis(this), Actions[0]->SectionID));
}

FText SVoxelMembers::GetFilterText() const
{
	return FilterBox->GetText();
}

TSharedPtr<SWidget> SVoxelMembers::OnContextMenuOpening()
{
	FMenuBuilder MenuBuilder(true, CommandList);

	if (const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction())
	{
		BaseAction->GetContextMenuActions(MenuBuilder);
		return MenuBuilder.MakeWidget();
	}

	if (WeakObject.IsValid())
	{
		MenuBuilder.BeginSection("BasicOperations");
		{
			MenuBuilder.AddMenuEntry(FGenericCommands::Get().Paste);
		}
		MenuBuilder.EndSection();

		MenuBuilder.BeginSection("AddNewItem", INVTEXT("Add New"));
		{
			GetContextMenuAddOptions(MenuBuilder);
		}
		MenuBuilder.EndSection();
	}

	return MenuBuilder.MakeWidget();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelMembers::OnDeleteEntry()
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction();
	if (!BaseAction)
	{
		return;
	}

	BaseAction->OnDelete();
}

void SVoxelMembers::OnDuplicateAction()
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction();
	if (!BaseAction)
	{
		return;
	}

	BaseAction->OnDuplicate();
}

void SVoxelMembers::OnRequestRenameOnActionNode()
{
	MembersMenu->OnRequestRenameOnActionNode();
}

bool SVoxelMembers::CanRequestRenameOnActionNode() const
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction();
	if (!BaseAction)
	{
		return false;
	}

	return
		BaseAction->CanRequestRename() &&
		MembersMenu->CanRequestRenameOnActionNode();
}

void SVoxelMembers::OnCopy()
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction();
	if (!BaseAction)
	{
		return;
	}

	if (!GetCopyPrefixes().IsValidIndex(BaseAction->SectionID))
	{
		return;
	}

	FString ExportText;
	if (!BaseAction->OnCopy(ExportText))
	{
		return;
	}

	ExportText = GetCopyPrefixes()[BaseAction->SectionID] + ExportText;

	FPlatformApplicationMisc::ClipboardCopy(*ExportText);
}

void SVoxelMembers::OnCut()
{
	OnCopy();
	OnDeleteEntry();
}

void SVoxelMembers::OnPaste()
{
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);

	int32 SectionId = -1;
	if (!UpdatePasteData(ClipboardText, SectionId))
	{
		return;
	}

	OnPasteItem(ClipboardText, SectionId);
}

bool SVoxelMembers::CanPaste()
{
	FString ClipboardText;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardText);

	int32 SectionId = -1;
	if (!UpdatePasteData(ClipboardText, SectionId))
	{
		return false;
	}

	return CanPasteItem(ClipboardText, SectionId);
}

void SVoxelMembers::OnFindReferences() const
{
	const TSharedPtr<FVoxelMembersBaseSchemaAction> BaseAction = GetAction();
	if (!BaseAction)
	{
		return;
	}

	const TSharedPtr<FVoxelGraphToolkit> Toolkit = GetToolkit<FVoxelGraphToolkit>();
	if (!ensure(Toolkit))
	{
		return;
	}

	Toolkit->GetSearchWidget()->FocusForUse(BaseAction->GetSearchString());
}

bool SVoxelMembers::UpdatePasteData(FString& ImportText, int32& OutSectionId) const
{
	for (int32 Index = 0; Index < GetCopyPrefixes().Num(); Index++)
	{
		if (ImportText.RemoveFromStart(GetCopyPrefixes()[Index], ESearchCase::CaseSensitive))
		{
			OutSectionId = Index;
			return true;
		}
	}

	return false;
}

void SVoxelMembers::OnFilterTextChanged(const FText& InFilterText) const
{
	MembersMenu->GenerateFilteredItems(false);
}