// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelToolkit.h"
#include "SGraphActionMenu.h"
#include "SchemaActions/VoxelMembersBaseSchemaAction.h"

class VOXELGRAPHEDITOR_API FVoxelMembersColumnSizeData
{
public:
	DECLARE_DELEGATE_ThreeParams(FOnMembersSlotResized, float, int32, float)
	DECLARE_DELEGATE_RetVal_TwoParams(float, FGetMembersColumnWidth, int32, float)

public:
	FVoxelMembersColumnSizeData()
	{
		ValueColumnWidthValue = 0.4f;
		HoveredSplitterIndexValue = INDEX_NONE;
		IndentationValue = 10.f;

		NameColumnWidth.BindRaw(this, &FVoxelMembersColumnSizeData::GetNameColumnWidth);
		ValueColumnWidth.BindRaw(this, &FVoxelMembersColumnSizeData::GetValueColumnWidth);
		HoveredSplitterIndex.BindRaw(this, &FVoxelMembersColumnSizeData::GetHoveredSplitterIndex);
		OnValueColumnResized.BindRaw(this, &FVoxelMembersColumnSizeData::SetValueColumnWidth);
		OnSplitterHandleHovered.BindRaw(this, &FVoxelMembersColumnSizeData::OnSetHoveredSplitterIndex);

		OnNameColumnResized.BindLambda([](float) {});
	}

	FGetMembersColumnWidth NameColumnWidth;
	FGetMembersColumnWidth ValueColumnWidth;
	TAttribute<int32> HoveredSplitterIndex;
	SSplitter::FOnSlotResized OnNameColumnResized;
	FOnMembersSlotResized OnValueColumnResized;
	SSplitter::FOnHandleHovered OnSplitterHandleHovered;

	void SetValueColumnWidth(float NewWidth, const int32 Indentation, const float CurrentSize)
	{
		if (CurrentSize != 0.f &&
			Indentation != 0)
		{
			NewWidth /= (CurrentSize + Indentation * IndentationValue) / CurrentSize;
		}

		ensureAlways(NewWidth <= 1.0f);
		ValueColumnWidthValue = NewWidth;
	}

private:
	float ValueColumnWidthValue;
	int32 HoveredSplitterIndexValue;
	float IndentationValue;

	float GetNameColumnWidth(const int32 Indentation, const float CurrentSize) const
	{
		return 1.f - GetValueColumnWidth(Indentation, CurrentSize);
	}

	float GetValueColumnWidth(const int32 Indentation, const float CurrentSize) const
	{
		if (CurrentSize == 0.f ||
			Indentation == 0)
		{
			return ValueColumnWidthValue;
		}

		return (CurrentSize + Indentation * IndentationValue) / CurrentSize * ValueColumnWidthValue;
	}
	int32 GetHoveredSplitterIndex() const
	{
		return HoveredSplitterIndexValue;
	}

	void OnSetHoveredSplitterIndex(const int32 HoveredIndex)
	{
		HoveredSplitterIndexValue = HoveredIndex;
	}
};

struct VOXELGRAPHEDITOR_API FVoxelMembersActionsSortHelper
{
	void AddCategoriesSortList(const int32 SectionId, const TArray<FString>& Categories);
	void AddAction(const TSharedRef<FEdGraphSchemaAction>& NewAction, const FString& Category);
	void GetAllActions(FGraphActionListBuilderBase& OutAllActions);

private:
	TMap<int32, TArray<FString>> SortedCategories;
	TMap<int32, TMap<FString, TArray<TSharedRef<FEdGraphSchemaAction>>>> Actions;
};

class VOXELGRAPHEDITOR_API SVoxelMembers
	: public SCompoundWidget
	, public FSelfRegisteringEditorUndoClient
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UObject*, Object);
		SLATE_ARGUMENT(TSharedPtr<FVoxelToolkit>, Toolkit);
	};

	void Construct(const FArguments& Args);

	//~ Begin SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End SWidget Interface

	void RequestRefresh();
	void SelectMember(FName Name, int32 SectionId, bool bRequestRename, bool bRefresh);
	FVoxelMembersColumnSizeData& GetColumnSizeData();

protected:
	//~ Begin Interface FSelfRegisteringEditorUndoClient
	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	//~ End Interface FSelfRegisteringEditorUndoClient

public:
	UObject* GetObject() const
	{
		return WeakObject.Get();
	}
	template<typename Type>
	Type* GetObject() const
	{
		return Cast<Type>(WeakObject.Get());
	}

	TSharedPtr<FVoxelToolkit> GetToolkit() const
	{
		return WeakToolkit.Pin();
	}
	template<typename Type, typename = typename TEnableIf<TIsDerivedFrom<Type, FVoxelToolkit>::Value>::Type>
	TSharedPtr<Type> GetToolkit() const
	{
		return Cast<Type>(WeakToolkit.Pin());
	}

	void SetObject(const TWeakObjectPtr<UObject>& Object)
	{
		WeakObject = Object;
	}

protected:
	virtual void CollectStaticSections(TArray<int32>& StaticSectionIds) VOXEL_PURE_VIRTUAL();
	virtual FText OnGetSectionTitle(int32 SectionId) VOXEL_PURE_VIRTUAL({});
	virtual TSharedRef<SWidget> OnGetMenuSectionWidget(TSharedRef<SWidget> RowWidget, int32 SectionId) VOXEL_PURE_VIRTUAL(SNullWidget::NullWidget);
	virtual void CollectSortedActions(FVoxelMembersActionsSortHelper& OutActionsList) VOXEL_PURE_VIRTUAL();
	virtual void GetContextMenuAddOptions(FMenuBuilder& MenuBuilder) VOXEL_PURE_VIRTUAL();
	virtual void OnPasteItem(const FString& ImportText, int32 SectionId) VOXEL_PURE_VIRTUAL();
	virtual bool CanPasteItem(const FString& ImportText, int32 SectionId) VOXEL_PURE_VIRTUAL(false);
	virtual void OnAddNewMember(int32 SectionId) VOXEL_PURE_VIRTUAL();
	virtual void SelectBaseObject() VOXEL_PURE_VIRTUAL();
	virtual const TArray<FString>& GetCopyPrefixes() const
	{
		static TArray<FString> Prefixes;
		return Prefixes;
	}

public:
	virtual TArray<FString>& GetEditableCategories(int32 SectionId)
	{
		static TArray<FString> CategoriesList;
		return CategoriesList;
	}

protected:
	void Refresh();
	FString GetPasteCategory() const;
	TSharedRef<SWidget> CreateAddButton(int32 SectionId, const FText& AddNewText, FName MetaDataTag);

	TSharedPtr<FVoxelMembersBaseSchemaAction> GetAction(const TSharedPtr<FEdGraphSchemaAction>& Action) const;
	TSharedPtr<FVoxelMembersBaseSchemaAction> GetAction(const TArray<TSharedPtr<FEdGraphSchemaAction>>& Actions) const;
	TSharedPtr<FVoxelMembersBaseSchemaAction> GetAction() const;

private:
	void CollectAllActions(FGraphActionListBuilderBase& OutAllActions);
	TSharedRef<SWidget> OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData) const;
	void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, ESelectInfo::Type InSelectionType);
	bool HandleActionMatchesName(FEdGraphSchemaAction* EdGraphSchemaAction, const FName& Name);
	FReply OnActionDragged(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions, const FPointerEvent& MouseEvent);
	void OnMemberActionDoubleClicked(const TArray<TSharedPtr<FEdGraphSchemaAction>>& InActions);

	void OnCategoryNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit, TWeakPtr< FGraphActionNode > InAction);
	FReply OnCategoryDragged(const FText& InCategory, const FPointerEvent& MouseEvent);
	FText GetFilterText() const;
	TSharedPtr<SWidget> OnContextMenuOpening();

private:
	void OnDeleteEntry();
	void OnDuplicateAction();

	void OnRequestRenameOnActionNode();
	bool CanRequestRenameOnActionNode() const;

	void OnCopy();
	void OnCut();

	void OnPaste();
	bool CanPaste();

	void OnFindReferences() const;

	bool UpdatePasteData(FString& ImportText, int32& OutSectionId) const;

	void OnFilterTextChanged(const FText& InFilterText) const;

private:
	bool bNeedsRefresh = false;
	bool bIsRefreshing = false;

	TSharedPtr<SSearchBox> FilterBox;
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<SGraphActionMenu> MembersMenu;

	TWeakObjectPtr<UObject> WeakObject;
	TWeakPtr<FVoxelToolkit> WeakToolkit;

	FVoxelMembersColumnSizeData ColumnSizeData;
};