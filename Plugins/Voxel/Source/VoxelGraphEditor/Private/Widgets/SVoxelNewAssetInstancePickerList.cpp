// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelNewAssetInstancePickerList.h"

FText SVoxelNewAssetInstancePickerList::VoxelExamplesCategory = INVTEXT("Voxel Examples");
FText SVoxelNewAssetInstancePickerList::ProjectCategory = INVTEXT("Project");
FText SVoxelNewAssetInstancePickerList::UncategorizedCategory = INVTEXT("Uncategorized");

void SVoxelNewAssetInstancePickerList::Construct(const FArguments& InArgs, UClass* AssetClass)
{
	OnTemplateAssetActivatedDelegate = InArgs._OnTemplateAssetActivated;
	OnGetAssetCategoryDelegate = InArgs._OnGetAssetCategory;
	OnGetAssetDescriptionDelegate = InArgs._OnGetAssetDescription;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(3.f)
		[
			SAssignNew(FilterBox, SVoxelNewAssetInstanceFilterBox)
			.OnSourceFiltersChanged(this, &SVoxelNewAssetInstancePickerList::TriggerRefresh)
			.Class(AssetClass)
		]
		+ SVerticalBox::Slot()
		[
			SAssignNew(ItemSelector, SAssetItemSelector)
			.Items(GetAssetDataForSelector(AssetClass))
			.AllowMultiselect(false)
			.ClickActivateMode(EItemSelectorClickActivateMode::DoubleClick)
			.OnGetCategoriesForItem(this, &SVoxelNewAssetInstancePickerList::OnGetCategoriesForItem)
			.OnGetSectionsForItem(this, &SVoxelNewAssetInstancePickerList::OnGetSectionsForItem)
			.OnCompareCategoriesForEquality_Lambda([](const FText& CategoryA, const FText& CategoryB)
			{
				return CategoryA.CompareTo(CategoryB) == 0;
			})
			.OnCompareSectionsForEquality_Lambda([](const FText& CategoryA, const FText& CategoryB)
			{
				return CategoryA.CompareTo(CategoryB) == 0;
			})
			.OnCompareSectionsForSorting_Lambda([](const FText& CategoryA, const FText& CategoryB)
			{
				const int32 A = SectionNameToValue(CategoryA);
				const int32 B = SectionNameToValue(CategoryB);

				if (A == B)
				{
					return CategoryA.CompareTo(CategoryB) < 0;
				}
				return A < B;
			})
			.OnCompareItemsForSorting_Lambda([](const FAssetData& ItemA, const FAssetData& ItemB)
			{
				return GetAssetName(ItemA) < GetAssetName(ItemB);
			})
			.OnDoesItemMatchFilterText_Lambda([](const FText& FilterText, const FAssetData& Item)
			{
				TArray<FString> FilterStrings;
				FilterText.ToString().ParseIntoArrayWS(FilterStrings, TEXT(","));

				const FString AssetNameString = GetAssetName(Item);
				for (const FString& FilterString : FilterStrings)
				{
					if (!AssetNameString.Contains(FilterString))
					{
						return false;
					}
				}

				return true;
			})
			.OnGenerateWidgetForCategory(this, &SVoxelNewAssetInstancePickerList::OnGenerateWidgetForCategory)
			.OnGenerateWidgetForSection(this, &SVoxelNewAssetInstancePickerList::OnGenerateWidgetForSection)
			.OnGenerateWidgetForItem(this, &SVoxelNewAssetInstancePickerList::OnGenerateWidgetForItem)
			.OnItemActivated_Lambda([&](const FAssetData& Item)
			{
				OnTemplateAssetActivatedDelegate.ExecuteIfBound(Item);
			})
			.OnDoesItemPassCustomFilter_Lambda([&](const FAssetData& Item)
			{
				return FilterBox->IsSourceFilterActive(Item);
			})
			.OnGetSectionData_Lambda([](const FText& Section)
			{
				if (Section.IsEmpty())
				{
					return SAssetItemSelector::FSectionData(SAssetItemSelector::FSectionData::List, true);
				}

				return SAssetItemSelector::FSectionData();
			})
			.ExpandInitially(true)
		]
	];

	FSlateApplication::Get().SetKeyboardFocus(ItemSelector->GetSearchBox());
}

TArray<FAssetData> SVoxelNewAssetInstancePickerList::GetSelectedAssets() const
{
	return ItemSelector->GetSelectedItems();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TArray<FText> SVoxelNewAssetInstancePickerList::OnGetCategoriesForItem(const FAssetData& Item) const
{
	TArray<FText> Categories;

	if (OnGetAssetCategoryDelegate.IsBound())
	{
		const FString CategoriesString = OnGetAssetCategoryDelegate.Execute(Item).ToString();

		TArray<FString> CategoriesList;
		CategoriesString.ParseIntoArray(CategoriesList, TEXT("|"));

		for (int32 Index = 0; Index < CategoriesList.Num(); Index++)
		{
			if (Index == 0 &&
				CategoriesList[0] == "Default")
			{
				continue;
			}

			if (CategoriesList[Index].IsEmpty())
			{
				continue;
			}

			Categories.Add(FText::FromString(FName::NameToDisplayString(CategoriesList[Index], false)));
		}
	}

	return Categories;
}

TArray<FText> SVoxelNewAssetInstancePickerList::OnGetSectionsForItem(const FAssetData& Item) const
{
	TArray<FText> Sections;

	TArray<FString> AssetPathParts;
	Item.GetSoftObjectPath().ToString().ParseIntoArray(AssetPathParts, TEXT("/"));

	if (ensure(AssetPathParts.Num() > 0))
	{
		if (AssetPathParts[0] == TEXT("Voxel"))
		{
			Sections.Add({});
		}
		else if (AssetPathParts[0] == TEXT("Game"))
		{
			if (AssetPathParts.IsValidIndex(1) &&
				AssetPathParts[1] == "VoxelExamples")
			{
				Sections.Add(VoxelExamplesCategory);
			}
			else
			{
				Sections.Add(ProjectCategory);
			}
		}
		else
		{
			Sections.Add(FText::FromString("Plugin - " + AssetPathParts[0]));
		}
	}
	else
	{
		Sections.Add(UncategorizedCategory);
	}

	return Sections;
}

TSharedRef<SWidget> SVoxelNewAssetInstancePickerList::OnGenerateWidgetForCategory(const FText& Category) const
{
	return
		SNew(SBox)
		.Padding(FMargin(5.f, 5.f, 5.f, 3.f))
		[
			SNew(STextBlock)
			.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.AssetPickerAssetCategoryText")
			.Text(Category)
		];
}

TSharedRef<SWidget> SVoxelNewAssetInstancePickerList::OnGenerateWidgetForSection(const FText& Section) const
{
	if (Section.IsEmpty())
	{
		return SNullWidget::NullWidget;
	}

	return
		SNew(SBox)
		.Padding(FMargin(5.f, 5.f, 5.f, 3.f))
		[
			SNew(STextBlock)
			.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.AssetPickerAssetSectionText")
			.Text(Section)
		];
}

TSharedRef<SWidget> SVoxelNewAssetInstancePickerList::OnGenerateWidgetForItem(const FAssetData& Item)
{
	const TSharedRef<FAssetThumbnail> AssetThumbnail = MakeVoxelShared<FAssetThumbnail>(Item, 72, 72, FVoxelEditorUtilities::GetThumbnailPool());

	FAssetThumbnailConfig ThumbnailConfig;
	ThumbnailConfig.bAllowFadeIn = false;

	FText AssetDescription;
	if (OnGetAssetDescriptionDelegate.IsBound())
	{
		AssetDescription = OnGetAssetDescriptionDelegate.Execute(Item);
	}

	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(15.f, 3.f, 10.f, 3.f)
		[
			SNew(SBox)
			.WidthOverride(72.f)
			.HeightOverride(72.f)
			[
				AssetThumbnail->MakeThumbnailWidget(ThumbnailConfig)
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(5.f, 0.f)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.AssetPickerBoldAssetNameText")
				.Text(FText::FromString(FName::NameToDisplayString(GetAssetName(Item), false)))
				.HighlightText_Lambda([&]
				{
					return ItemSelector->GetFilterText();
				})
			]
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(AssetDescription)
				.TextStyle(FVoxelEditorStyle::Get(), "Graph.NewAssetDialog.AssetPickerAssetNameText")
				.AutoWrapText(true)
			]
		];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TArray<FAssetData> SVoxelNewAssetInstancePickerList::GetAssetDataForSelector(const UClass* AssetClass)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	TArray<FAssetData> Assets;
	
	AssetRegistryModule.Get().GetAssetsByClass(AssetClass->GetClassPathName(), Assets);
	return Assets;
}

void SVoxelNewAssetInstancePickerList::TriggerRefresh(const TMap<EVoxelGraphScriptSource, bool>& SourceState) const
{
	ItemSelector->RefreshAllCurrentItems();
	ItemSelector->ExpandTree();
}

FString SVoxelNewAssetInstancePickerList::GetAssetName(const FAssetData& Item)
{
	FString Result = Item.AssetName.ToString();
	Result.RemoveFromStart("VG_");
	Result.RemoveFromStart("VGI_");
	return Result;
}

int32 SVoxelNewAssetInstancePickerList::SectionNameToValue(const FText& Category)
{
	if (Category.IsEmpty())
	{
		return 0;
	}

	if (Category.EqualTo(VoxelExamplesCategory))
	{
		return 1;
	}

	if (Category.EqualTo(ProjectCategory))
	{
		return 2;
	}

	if (Category.EqualTo(UncategorizedCategory))
	{
		return 4;
	}

	return 3;
}