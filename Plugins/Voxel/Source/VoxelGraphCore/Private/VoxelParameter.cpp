// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameter.h"
#include "VoxelGraph.h"
#include "VoxelChannel.h"
#include "VoxelChannelAsset_DEPRECATED.h"

void FVoxelParameter::Fixup(UObject* Outer)
{
	if (Type.Is<FVoxelChannelRef_DEPRECATED>())
	{
		Type = FVoxelPinType::Make<FVoxelChannelName>();

		if (ensure(DefaultValue.IsObject()))
		{
			const UVoxelChannelAsset_DEPRECATED* Channel = Cast<UVoxelChannelAsset_DEPRECATED>(DefaultValue.GetObject());
			if (ensure(Channel))
			{
				DefaultValue = FVoxelPinValue::Make(Channel->MakeChannelName());
			}
		}
	}

	if (!Type.IsValid())
	{
		Type = FVoxelPinType::Make<float>();
	}

	DefaultValue.Fixup(Type.GetExposedType(), Outer);
}

bool FVoxelParameter::Identical(const FVoxelParameter& Other, bool bCheckDefaultValue) const
{
	if (Guid != Other.Guid ||
		Name != Other.Name ||
		Type != Other.Type ||
		Category != Other.Category ||
		Description != Other.Description)
	{
		return false;
	}

#if WITH_EDITOR
	if (!MetaData.OrderIndependentCompareEqual(Other.MetaData))
	{
		return false;
	}
#endif

	if (bCheckDefaultValue &&
		DefaultValue != Other.DefaultValue)
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterCategories::Fixup(const TArray<FString>& CategoriesList)
{
	VOXEL_FUNCTION_COUNTER();

	struct FChainedCategory
	{
		FString Name;
		TArray<FChainedCategory> InnerCategories;

		explicit FChainedCategory(const FString& Name)
			: Name(Name)
		{

		}

		bool operator==(const FString& OtherName) const
		{
			return Name == OtherName;
		}

		TArray<FString> GetCategories() const
		{
			TArray<FString> ResultCategories;
			for (const FChainedCategory& InnerCategory : InnerCategories)
			{
				TArray<FString> PreparedCategories = InnerCategory.GetCategories();
				for (const FString& PreparedCategory : PreparedCategories)
				{
					ResultCategories.Add(Name + "|" + PreparedCategory);
				}
			}
			ResultCategories.Add(Name);
			return ResultCategories;
		}
	};

	const auto AddChainedCategory = [](const FString& CategoryName, TArray<FChainedCategory>& TargetCategories)
	{
		FChainedCategory* TargetCategory = TargetCategories.FindByKey(CategoryName);
		if (!TargetCategory)
		{
			TargetCategory = &TargetCategories[TargetCategories.Add(FChainedCategory(CategoryName))];
		}
		return TargetCategory;
	};

	TSet<FString> UsedCategories;

	// Add missing categories and fill used categories
	for (const FString& ExistingCategory : CategoriesList)
	{
		UsedCategories.Add(ExistingCategory);
		Categories.AddUnique(ExistingCategory);
	}

	// Remove unused categories
	Categories.RemoveAll([&](const FString& InCategory)
	{
		return !UsedCategories.Contains(InCategory);
	});

	// Move subcategories before higher level categories
	TArray<FChainedCategory> TopLevelCategories;

	TArray<FString> CopiedCategories = Categories;
	Categories = {};

	// Buildup chained categories for proper ordering
	for (const FString& InCategory : CopiedCategories)
	{
		TArray<FString> CategoryChain;
		InCategory.ParseIntoArray(CategoryChain, TEXT("|"), true);

		FChainedCategory* TargetCategory = nullptr;
		for (int32 Index = 0; Index < CategoryChain.Num(); Index++)
		{
			if (Index == 0)
			{
				TargetCategory = AddChainedCategory(CategoryChain[Index], TopLevelCategories);
			}
			else
			{
				TargetCategory = AddChainedCategory(CategoryChain[Index], TargetCategory->InnerCategories);
			}
		}
	}

	// Re add all existing chained categories
	for (FChainedCategory& InCategory : TopLevelCategories)
	{
		for (const FString& GeneratedCategory : InCategory.GetCategories())
		{
			if (UsedCategories.Contains(GeneratedCategory))
			{
				Categories.Add(GeneratedCategory);
			}
		}
	}
}