// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "VoxelMacroLibrary.h"

VOXEL_CUSTOMIZE_CLASS(UVoxelGraph)(IDetailLayoutBuilder& DetailLayout)
{
	const TSharedRef<IPropertyHandle> Parameters = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, Parameters));
	Parameters->MarkHiddenByCustomization();

	const TSharedRef<IPropertyHandle> ParameterGraphsHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, ParameterGraphs));
	ParameterGraphsHandle->MarkHiddenByCustomization();

	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	DetailLayout.GetObjectsBeingCustomized(SelectedObjects);
	if (SelectedObjects.Num() != 1)
	{
		return;
	}

	const UVoxelGraph* Graph = Cast<UVoxelGraph>(SelectedObjects[0].Get());
	if (!ensure(Graph))
	{
		return;
	}

	const TSharedRef<IPropertyHandle> EnableOverrideHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, bEnableNameOverride));
	EnableOverrideHandle->MarkHiddenByCustomization();

	const TSharedRef<IPropertyHandle> NameHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, NameOverride));
	IDetailPropertyRow& NameRow = DetailLayout.AddPropertyToCategory(NameHandle);

	const TSharedRef<IPropertyHandle> CategoryHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, Category), UVoxelGraph::StaticClass());
	IDetailPropertyRow& CategoryRow = DetailLayout.AddPropertyToCategory(CategoryHandle);

	const TSharedRef<IPropertyHandle> ExposeToLibraryHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraph, bExposeToLibrary));
	if (!Graph->GetTypedOuter<UVoxelMacroLibrary>())
	{
		ExposeToLibraryHandle->MarkHiddenByCustomization();
	}

	const UVoxelGraph* MainGraph = Graph->GetTypedOuter<UVoxelGraph>();
	if (!MainGraph)
	{
		NameRow.EditCondition(MakeAttributeLambda([EnableOverrideHandle]
		{
			bool bEnabled = false;
			EnableOverrideHandle->GetValue(bEnabled);

			return bEnabled;
		}),
		MakeLambdaDelegate([EnableOverrideHandle](const bool bNewValue)
		{
			EnableOverrideHandle->SetValue(bNewValue);
		}));

		return;
	}

	// Create category selection
	FDetailWidgetRow& CustomWidget = CategoryRow.CustomWidget();

	FString Category;
	CategoryHandle->GetValue(Category);

	CustomWidget
	.NameContent()
	[
		CategoryHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SBox)
		.MinDesiredWidth(125.f)
		[
			SNew(SVoxelDetailComboBox<FString>)
			.RefreshDelegate(CategoryHandle, DetailLayout)
			.Options_Lambda([WeakMainGraph = MakeWeakObjectPtr(MainGraph)]()
			{
				TArray<FString> Categories;

				if (const UVoxelGraph* PinnedMainGraph = WeakMainGraph.Get())
				{
					Categories.Append(PinnedMainGraph->InlineMacroCategories.Categories);
				}

				Categories.AddUnique("Default");
				return Categories;
			})
			.CurrentOption(Category.IsEmpty() ? "Default" : Category)
			.CanEnterCustomOption(true)
			.OptionText(MakeLambdaDelegate([](FString Option)
			{
				return Option;
			}))
			.OnSelection_Lambda([CategoryHandle](const FString NewValue)
			{
				CategoryHandle->SetValue(NewValue == "Default" ? "" : NewValue);
			})
		]
	];
}