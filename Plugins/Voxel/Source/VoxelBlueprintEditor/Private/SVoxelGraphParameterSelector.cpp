// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphParameterSelector.h"
#include "VoxelGraphVisuals.h"
#include "VoxelParameter.h"
#include "VoxelParameterView.h"
#include "SListViewSelectorDropdownMenu.h"

struct FSelectorParameterRow
{
public:
	FString Name;
	FVoxelParameter Parameter;
	TArray<TSharedPtr<FSelectorParameterRow>> Parameters;
	TMap<FName, TSharedPtr<FSelectorParameterRow>> ChildCategories;

	FSelectorParameterRow() = default;

	explicit FSelectorParameterRow(const FVoxelParameter& Parameter)
		: Name(Parameter.Name.ToString())
		, Parameter(Parameter)
	{
	}

	explicit FSelectorParameterRow(const FString& Name)
		: Name(Name)
	{
	}

	FSelectorParameterRow(const FString& Name, const TArray<TSharedPtr<FSelectorParameterRow>>& Types)
		: Name(Name)
		, Parameters(Types)
	{
	}
};

void SVoxelGraphParameterSelector::Construct(const FArguments& InArgs)
{
	CachedParameterProvider = InArgs._ParameterProvider;

	OnParameterChanged = InArgs._OnParameterChanged;
	ensure(OnParameterChanged.IsBound());
	OnCloseMenu = InArgs._OnCloseMenu;

	FillParametersList();

	SAssignNew(ParametersTreeView, SParameterTreeView)
	.TreeItemsSource(&FilteredParametersList)
	.SelectionMode(ESelectionMode::Single)
	.OnGenerateRow(this, &SVoxelGraphParameterSelector::GenerateParameterTreeRow)
	.OnSelectionChanged(this, &SVoxelGraphParameterSelector::OnParameterSelectionChanged)
	.OnGetChildren(this, &SVoxelGraphParameterSelector::GetParameterChildren);

	ParametersTreeView->SetSingleExpandedItem(AutoExpandedRow.Pin());

	SAssignNew(FilterTextBox, SSearchBox)
	.OnTextChanged(this, &SVoxelGraphParameterSelector::OnFilterTextChanged)
	.OnTextCommitted(this, &SVoxelGraphParameterSelector::OnFilterTextCommitted);

	ChildSlot
	[
		SNew(SListViewSelectorDropdownMenu<TSharedPtr<FSelectorParameterRow>>, FilterTextBox, ParametersTreeView)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(4.f)
			[
				FilterTextBox.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(4.f)
			[
				SNew(SBox)
				.HeightOverride(400.f)
				.WidthOverride(300.f)
				[
					ParametersTreeView.ToSharedRef()
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.f, 0.f, 8.f, 4.f)
			[
				SNew(STextBlock)
				.Text_Lambda([this]
				{
					const FString ItemText = FilteredParametersCount == 1 ? " parameter" : " parameters";
					return FText::FromString(FText::AsNumber(FilteredParametersCount).ToString() + ItemText);
				})
			]
		]
	];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphParameterSelector::ClearSelection() const
{
	ParametersTreeView->SetSelection(nullptr, ESelectInfo::OnNavigation);
	ParametersTreeView->SetSingleExpandedItem(AutoExpandedRow.Pin());
}

TSharedPtr<SWidget> SVoxelGraphParameterSelector::GetWidgetToFocus() const
{
	return FilterTextBox;
}

void SVoxelGraphParameterSelector::UpdateParameterProvider(const TWeakInterfacePtr<IVoxelParameterProvider>& ParameterProvider)
{
	CachedParameterProvider = ParameterProvider;

	FillParametersList();
}

FVoxelParameter SVoxelGraphParameterSelector::GetUpdatedParameter(const FVoxelParameter& TargetParameter) const
{
	IVoxelParameterProvider* ParameterProvider = CachedParameterProvider.Get();
	if (!ParameterProvider)
	{
		return {};
	}

	const TSharedPtr<IVoxelParameterRootView> ParameterRootView = ParameterProvider->GetParameterView();
	if (!ensure(ParameterRootView))
	{
		return {};
	}

	const IVoxelParameterView* ParameterView = ParameterRootView->FindByGuid(TargetParameter.Guid);
	if (!ParameterView)
	{
		return {};
	}

	return ParameterView->GetAsParameter();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<ITableRow> SVoxelGraphParameterSelector::GenerateParameterTreeRow(TSharedPtr<FSelectorParameterRow> RowItem, const TSharedRef<STableViewBase>& OwnerTable) const
{
	const FLinearColor Color = RowItem->Parameters.Num() > 0 ? FLinearColor::White : GetColor(RowItem->Parameter.Type);
	const FSlateBrush* Image = RowItem->Parameters.Num() > 0 ? FAppStyle::GetBrush("NoBrush") : GetIcon(RowItem->Parameter.Type);

	const TSharedRef<SHorizontalBox> Row =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .Padding(1.f)
		[
			SNew(SImage)
			.Image(Image)
			.ColorAndOpacity(Color)
		]
		+ SHorizontalBox::Slot()
		  .AutoWidth()
		  .Padding(1.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(RowItem->Name))
			.HighlightText_Lambda([this]
			{
				return SearchText;
			})
			.Font(RowItem->Parameters.Num() > 0 ? FAppStyle::GetFontStyle(TEXT("Kismet.TypePicker.CategoryFont")) : FAppStyle::GetFontStyle(TEXT("Kismet.TypePicker.NormalFont")))
		];

	if (RowItem->Parameters.Num() == 0)
	{
		Row->AddSlot()
			.AutoWidth()
			.Padding(5.f, 1.f, 1.f, 1.f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("(" + RowItem->Parameter.Type.ToString() + ")"))
				.Font(FAppStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont")))
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			];
	}

	return
		SNew(STableRow<TSharedPtr<FSelectorParameterRow>>, OwnerTable)
		[
			Row
		];
}

void SVoxelGraphParameterSelector::OnParameterSelectionChanged(TSharedPtr<FSelectorParameterRow> Selection, ESelectInfo::Type SelectInfo) const
{
	if (SelectInfo == ESelectInfo::OnNavigation)
	{
		if (ParametersTreeView->WidgetFromItem(Selection).IsValid())
		{
			OnCloseMenu.ExecuteIfBound();
		}

		return;
	}

	if (!Selection)
	{
		return;
	}

	if (Selection->Parameters.Num() == 0)
	{
		OnCloseMenu.ExecuteIfBound();
		OnParameterChanged.ExecuteIfBound(Selection->Parameter);
		return;
	}

	ParametersTreeView->SetItemExpansion(Selection, !ParametersTreeView->IsItemExpanded(Selection));

	if (SelectInfo == ESelectInfo::OnMouseClick)
	{
		ParametersTreeView->ClearSelection();
	}
}

void SVoxelGraphParameterSelector::GetParameterChildren(TSharedPtr<FSelectorParameterRow> ParameterRow, TArray<TSharedPtr<FSelectorParameterRow>>& ChildrenRows) const
{
	ChildrenRows = ParameterRow->Parameters;
}

void SVoxelGraphParameterSelector::OnFilterTextChanged(const FText& NewText)
{
	SearchText = NewText;
	FilteredParametersList = {};

	GetChildrenMatchingSearch(NewText);
	ParametersTreeView->RequestTreeRefresh();
}

void SVoxelGraphParameterSelector::OnFilterTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo) const
{
	if (CommitInfo != ETextCommit::OnEnter)
	{
		return;
	}

	TArray<TSharedPtr<FSelectorParameterRow>> SelectedItems = ParametersTreeView->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		ParametersTreeView->SetSelection(SelectedItems[0]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void SVoxelGraphParameterSelector::GetChildrenMatchingSearch(const FText& InSearchText)
{
	FilteredParametersCount = 0;

	TArray<FString> FilterTerms;
	TArray<FString> SanitizedFilterTerms;

	if (InSearchText.IsEmpty())
	{
		FilteredParametersList = ParametersList;
		FilteredParametersCount = TotalParametersCount;
		return;
	}

	FText::TrimPrecedingAndTrailing(InSearchText).ToString().ParseIntoArray(FilterTerms, TEXT(" "), true);

	for (int32 Index = 0; Index < FilterTerms.Num(); Index++)
	{
		FString EachString = FName::NameToDisplayString(FilterTerms[Index], false);
		EachString = EachString.Replace(TEXT(" "), TEXT(""));
		SanitizedFilterTerms.Add(EachString);
	}

	ensure(SanitizedFilterTerms.Num() == FilterTerms.Num());

	const auto SearchMatches = [&FilterTerms, &SanitizedFilterTerms](const TSharedPtr<FSelectorParameterRow>& TypeRow) -> bool
	{
		FString ItemName = TypeRow->Name;
		ItemName = ItemName.Replace(TEXT(" "), TEXT(""));

		for (int32 Index = 0; Index < FilterTerms.Num(); ++Index)
		{
			if (ItemName.Contains(FilterTerms[Index]) ||
				ItemName.Contains(SanitizedFilterTerms[Index]))
			{
				return true;
			}
		}

		return false;
	};

	const auto LookThroughList = [&](const TArray<TSharedPtr<FSelectorParameterRow>>& UnfilteredList, TArray<TSharedPtr<FSelectorParameterRow>>& OutFilteredList, auto& Lambda) -> bool
	{
		bool bReturnVal = false;
		for (const TSharedPtr<FSelectorParameterRow>& TypeRow : UnfilteredList)
		{
			const bool bMatchesItem = SearchMatches(TypeRow);
			if (TypeRow->Parameters.Num() == 0 ||
				bMatchesItem)
			{
				if (bMatchesItem)
				{
					OutFilteredList.Add(TypeRow);

					if (TypeRow->Parameters.Num() > 0 &&
						ParametersTreeView.IsValid())
					{
						ParametersTreeView->SetItemExpansion(TypeRow, true);
					}

					FilteredParametersCount += FMath::Max(1, TypeRow->Parameters.Num());

					bReturnVal = true;
				}
				continue;
			}

			TArray<TSharedPtr<FSelectorParameterRow>> ValidChildren;
			if (Lambda(TypeRow->Parameters, ValidChildren, Lambda))
			{
				TSharedRef<FSelectorParameterRow> NewCategory = MakeVoxelShared<FSelectorParameterRow>(TypeRow->Name, ValidChildren);
				OutFilteredList.Add(NewCategory);

				if (ParametersTreeView.IsValid())
				{
					ParametersTreeView->SetItemExpansion(NewCategory, true);
				}

				bReturnVal = true;
			}
		}

		return bReturnVal;
	};

	LookThroughList(ParametersList, FilteredParametersList, LookThroughList);
}

void SVoxelGraphParameterSelector::FillParametersList()
{
	ParametersList = {};

	IVoxelParameterProvider* ParameterProvider = CachedParameterProvider.Get();
	if (!ParameterProvider)
	{
		return;
	}

	const TSharedPtr<IVoxelParameterRootView> ParameterRootView = ParameterProvider->GetParameterView();
	if (!ensure(ParameterRootView))
	{
		return;
	}

	const TSharedPtr<FSelectorParameterRow> RootRow = MakeVoxelShared<FSelectorParameterRow>();

	{
		const TSharedPtr<FSelectorParameterRow> DefaultCategory = MakeVoxelShared<FSelectorParameterRow>("Default");
		RootRow->ChildCategories.Add(STATIC_FNAME("Default"), DefaultCategory);
		RootRow->Parameters.Add(DefaultCategory);
	}

	const FVoxelParameterCategories* Categories = ParameterRootView->GetCategories();
	if (ensure(Categories))
	{
		for (const FString& Category : Categories->Categories)
		{
			TArray<FString> CategoryChain = GetCategoryChain(Category);
			if (CategoryChain.Num() == 0)
			{
				continue;
			}

			TSharedPtr<FSelectorParameterRow> TargetRow = RootRow;
			for (int32 Index = 0; Index < CategoryChain.Num(); Index++)
			{
				if (const TSharedPtr<FSelectorParameterRow> CategoryRow = RootRow->ChildCategories.FindRef(*CategoryChain[Index]))
				{
					TargetRow = CategoryRow;
					continue;
				}

				TSharedPtr<FSelectorParameterRow> CategoryRow = MakeVoxelShared<FSelectorParameterRow>(CategoryChain[Index]);
				TargetRow->Parameters.Add(CategoryRow);
				TargetRow->ChildCategories.Add(*CategoryChain[Index], CategoryRow);

				TargetRow = CategoryRow;
			}
		}
	}

	const auto GetCategoryRow = [&](const FString& Category) -> TSharedPtr<FSelectorParameterRow>
	{
		TArray<FString> CategoryChain = GetCategoryChain(Category);
		if (CategoryChain.Num() == 0)
		{
			return RootRow->ChildCategories[STATIC_FNAME("Default")];
		}

		TSharedPtr<FSelectorParameterRow> TargetRow = RootRow;
		for (int32 Index = 0; Index < CategoryChain.Num(); Index++)
		{
			if (const TSharedPtr<FSelectorParameterRow> CategoryRow = TargetRow->ChildCategories.FindRef(*CategoryChain[Index]))
			{
				TargetRow = CategoryRow;
				continue;
			}

			TSharedPtr<FSelectorParameterRow> CategoryRow = MakeVoxelShared<FSelectorParameterRow>(CategoryChain[Index]);
			TargetRow->Parameters.Add(CategoryRow);
			TargetRow->ChildCategories.Add(*CategoryChain[Index], CategoryRow);

			TargetRow = CategoryRow;
		}

		return TargetRow;
	};

	int32 NumParameters = 0;
	for (const IVoxelParameterView* ParameterView : ParameterRootView->GetChildren())
	{
		const FVoxelParameter Parameter = ParameterView->GetAsParameter();

		if (const TSharedPtr<FSelectorParameterRow> CategoryRow = GetCategoryRow(Parameter.Category))
		{
			CategoryRow->Parameters.Add(MakeVoxelShared<FSelectorParameterRow>(Parameter));
		}
		else
		{
			ParametersList.Add(MakeVoxelShared<FSelectorParameterRow>(Parameter));
		}

		NumParameters++;
	}

	AutoExpandedRow = nullptr;
	for (const TSharedPtr<FSelectorParameterRow>& CategoryRow : RootRow->Parameters)
	{
		if (CategoryRow->Parameters.Num() == 0)
		{
			continue;
		}

		if (!AutoExpandedRow.IsValid())
		{
			AutoExpandedRow = CategoryRow;
		}

		ParametersList.Add(CategoryRow);
	}

	FilteredParametersList = ParametersList;

	TotalParametersCount = NumParameters;
	FilteredParametersCount = NumParameters;
}

const FSlateBrush* SVoxelGraphParameterSelector::GetIcon(const FVoxelPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinIcon(PinType).GetIcon();
}

FLinearColor SVoxelGraphParameterSelector::GetColor(const FVoxelPinType& PinType) const
{
	return FVoxelGraphVisuals::GetPinColor(PinType);
}

TArray<FString> SVoxelGraphParameterSelector::GetCategoryChain(const FString& Category) const
{
	TArray<FString> CategoryChain;
	FEditorCategoryUtils::GetCategoryDisplayString(Category).ParseIntoArray(CategoryChain, TEXT("|"), true);
	return CategoryChain;
}