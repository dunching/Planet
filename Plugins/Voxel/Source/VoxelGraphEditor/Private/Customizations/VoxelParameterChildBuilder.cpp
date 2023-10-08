// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Customizations/VoxelParameterChildBuilder.h"
#include "Customizations/VoxelParameterDetails.h"
#include "Customizations/VoxelParameterContainerDetails.h"
#include "VoxelParameterContainer.h"
#define private public
#include "Editor/PropertyEditor/Private/DetailItemNode.h"
#undef private

void FVoxelParameterChildBuilder::UpdateExpandedState()
{
	const TSharedPtr<const FDetailItemNode> Node = WeakNode.Pin();
	if (!Node)
	{
		return;
	}

#if INTELLISENSE_PARSER
	const bool bNewIsExpanded = this != nullptr;
#else
	const bool bNewIsExpanded = Node->bIsExpanded;
#endif
	if (bIsExpanded == bNewIsExpanded)
	{
		return;
	}

	bIsExpanded = bNewIsExpanded;

	if (!bIsExpanded)
	{
		VOXEL_SCOPE_COUNTER("Fixup DetailCustomWidgetExpansion");

		// Logic around DetailCustomWidgetExpansion is a bit funky,
		// and sometimes expanded items can be leaked to other actors/objects, making them always expanded.
		// To mitigate this, we force clear DetailCustomWidgetExpansion of anything containing our Path on collapse

		const FString Name = ParameterDetails.Path.ToString();
		TVoxelArray<FString> KeysToFix;

		GConfig->ForEachEntry(
			MakeLambdaDelegate([&](const TCHAR* Key, const TCHAR* Value)
			{
				if (FStringView(Value).Contains(Name))
				{
					KeysToFix.Add(Key);
				}
			}),
			TEXT("DetailCustomWidgetExpansion"),
			GEditorPerProjectIni);

		for (const FString& Key : KeysToFix)
		{
			FString ExpandedCustomItems;
			GConfig->GetString(TEXT("DetailCustomWidgetExpansion"), *Key, ExpandedCustomItems, GEditorPerProjectIni);

			TArray<FString> ExpandedCustomItemsArray;
			ExpandedCustomItems.ParseIntoArray(ExpandedCustomItemsArray, TEXT(","), true);

			ensure(ExpandedCustomItemsArray.RemoveAllSwap([&](const FString& Item)
			{
				return Item.Contains(Name);
			}));

			TStringBuilder<256> ExpandedCustomItemsBuilder;
			ExpandedCustomItemsBuilder.Join(ExpandedCustomItemsArray, TEXT(","));
			GConfig->SetString(TEXT("DetailCustomWidgetExpansion"), *Key, *ExpandedCustomItemsBuilder, GEditorPerProjectIni);
		}
	}

	(void)OnRegenerateChildren.ExecuteIfBound();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterChildBuilder::SetOnRebuildChildren(const FSimpleDelegate InOnRegenerateChildren)
{
	// Recover node from CreateSP in FDetailCustomBuilderRow::OnItemNodeInitialized
	const FDetailItemNode* Node = static_cast<const FDetailItemNode*>(InOnRegenerateChildren.GetObjectForTimerManager());
	if (!ensure(Node))
	{
		return;
	}

	WeakNode = Node->AsWeak();
	OnRegenerateChildren = InOnRegenerateChildren;
}

void FVoxelParameterChildBuilder::GenerateHeaderRowContent(FDetailWidgetRow& Row)
{
	ParameterDetails.BuildRow(Row, ValueWidget);
}

void FVoxelParameterChildBuilder::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	if (!ensure(!ParameterDetails.IsOrphan()))
	{
		return;
	}

	const bool bHasAnyOverride = INLINE_LAMBDA
	{
		for (const UVoxelParameterContainer* ParameterContainer : ParameterDetails.ContainerDetails.GetParameterContainers())
		{
			for (const auto& It : ParameterContainer->ValueOverrides)
			{
				if (It.Key.StartsWith(ParameterDetails.Path))
				{
					return true;
				}
			}
		}
		return false;
	};

	if (!bIsExpanded &&
		!bHasAnyOverride)
	{
		ChildrenBuilder.AddCustomRow({})
		.NameContent()
		[
			SNew(SVoxelDetailText)
			.Text(INVTEXT("Loading..."))
		];
		return;
	}

	ParameterViewsCommonChildren = IVoxelParameterViewBase::GetCommonChildren(ParameterDetails.ParameterViews);

	TMap<FName, TVoxelArray<TVoxelArray<IVoxelParameterView*>>> CategoryToAllChildParameterViews;
	for (const TVoxelArray<IVoxelParameterView*>& ChildParameterViews : ParameterViewsCommonChildren)
	{
		const FString Category = ChildParameterViews[0]->GetCategory();
		for (const IVoxelParameterView* ChildParameterView : ChildParameterViews)
		{
			ensure(ChildParameterView->GetCategory() == Category);
		}

		CategoryToAllChildParameterViews.FindOrAdd(FName(Category)).Add(ChildParameterViews);
	}

	for (UVoxelParameterContainer* ParameterContainer : ParameterDetails.ContainerDetails.GetParameterContainers())
	{
		for (const auto& It : ParameterContainer->ValueOverrides)
		{
			CategoryToAllChildParameterViews.FindOrAdd(It.Value.CachedCategory);
		}
	}

	if (const FVoxelParameterCategories* Categories = ParameterDetails.ParameterViews[0]->GetCategories())
	{
		const TArray<FName> CategoryArray(Categories->Categories);
		CategoryToAllChildParameterViews.KeySort([&](const FName CategoryA, const FName CategoryB)
		{
			return CategoryArray.IndexOfByKey(CategoryA) < CategoryArray.IndexOfByKey(CategoryB);
		});
	}

	const FVoxelDetailCategoryInterface DetailCategoryInterface(ChildrenBuilder);
	for (const auto& It : CategoryToAllChildParameterViews)
	{
		DetailCategoryInterface.EditCategory(It.Key, GetName().ToString() + "." + It.Key, MakeWeakPtrLambda(this,
			[=](const FVoxelDetailInterface& DetailInterface)
			{
				for (const TVoxelArray<IVoxelParameterView*>& ChildParameterViews : It.Value)
				{
					ParameterDetails.ContainerDetails.GenerateView(ChildParameterViews, DetailInterface);
				}

				ParameterDetails.ContainerDetails.AddOrphans(
					ParameterDetails.Path,
					ParameterDetails.ParameterViews,
					DetailInterface,
					It.Key);
			}));
	}
}

FName FVoxelParameterChildBuilder::GetName() const
{
	return FName(ParameterDetails.Path.ToString());
}