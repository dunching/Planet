// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "Customizations/VoxelPinValueCustomizationHelper.h"

VOXEL_CUSTOMIZE_STRUCT_CHILDREN(FVoxelGraphParameter)(const TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const EVoxelGraphParameterType ParameterType = FVoxelEditorUtilities::GetEnumPropertyValue<EVoxelGraphParameterType>(PropertyHandle->GetChildHandleStatic(FVoxelGraphParameter, ParameterType));

	ChildBuilder.AddProperty(PropertyHandle->GetChildHandleStatic(FVoxelGraphParameter, Name));

	const TSharedRef<IPropertyHandle> TypeHandle = PropertyHandle->GetChildHandleStatic(FVoxelGraphParameter, Type);
	if (ParameterType == EVoxelGraphParameterType::Parameter)
	{
		TypeHandle->SetInstanceMetaData(STATIC_FNAME("FilterTypes"), "AllUniformsAndBufferArrays");
	}
	ChildBuilder.AddProperty(TypeHandle);

	// Create category selection
	{
		const TSharedRef<IPropertyHandle> CategoryHandle = PropertyHandle->GetChildHandleStatic(FVoxelGraphParameter, Category);
		FString Category;
		CategoryHandle->GetValue(Category);

		ChildBuilder.AddCustomRow(INVTEXT("Category"))
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
				.RefreshDelegate(CategoryHandle, ChildBuilder)
				.Options_Lambda([=]()
				{
					TArray<FString> Categories;

					{
						TArray<UObject*> OuterObjects;
						PropertyHandle->GetOuterObjects(OuterObjects);
						if (OuterObjects.Num() == 1)
						{
							if (UVoxelGraph* Graph = Cast<UVoxelGraph>(OuterObjects[0]))
							{
								Categories = Graph->GetCategories(ParameterType);
							}
						}
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
				.OnSelection_Lambda([CategoryHandle](FString NewValue)
				{
					CategoryHandle->SetValue(NewValue == "Default" ? "" : NewValue);
				})
			]
		];
	}

	ChildBuilder.AddProperty(PropertyHandle->GetChildHandleStatic(FVoxelGraphParameter, Description));

	const auto GetMetaData = [](const TSharedPtr<IPropertyHandle>& Handle)
	{
		const FVoxelGraphParameter Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelGraphParameter>(Handle);
		return Parameter.MetaData;
	};
	const auto SetMetaData = [](const TSharedPtr<IPropertyHandle>& Handle, const TMap<FName, FString>& MetaData)
	{
		FVoxelGraphParameter Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelGraphParameter>(Handle);
		Parameter.MetaData = MetaData;
		FVoxelEditorUtilities::SetStructPropertyValue<FVoxelGraphParameter>(Handle, Parameter);
	};

	const FString* ShowDefaultValue = PropertyHandle->GetInstanceMetaData("ShowDefaultValue");
	if (!ensure(ShowDefaultValue))
	{
		return;
	}

	const bool bShowDefaultValue = *ShowDefaultValue == "true";
	ensure(bShowDefaultValue || *ShowDefaultValue == "false");

	if (!bShowDefaultValue)
	{
		return;
	}

	IDetailCategoryBuilder& DefaultValueCategory = ChildBuilder.GetParentCategory().GetParentLayout().EditCategory("Default Value", INVTEXT("Default Value"));

	FVoxelPinValueCustomizationHelper::CreatePinValueRangeSetter(
		DefaultValueCategory.AddCustomRow(INVTEXT("Slider Range")),
		PropertyHandle,
		INVTEXT("Slider Range"),
		INVTEXT("Allows setting the minimum and maximum values for the UI slider for this variable."),
		STATIC_FNAME("UIMin"),
		STATIC_FNAME("UIMax"),
		GetMetaData,
		SetMetaData);

	FVoxelPinValueCustomizationHelper::CreatePinValueRangeSetter(
		DefaultValueCategory.AddCustomRow(INVTEXT("Value Range")),
		PropertyHandle,
		INVTEXT("Value Range"),
		INVTEXT("The range of values allowed by this variable. Values outside of this will be clamped to the range."),
		STATIC_FNAME("ClampMin"),
		STATIC_FNAME("ClampMax"),
		GetMetaData,
		SetMetaData);

	const FVoxelGraphParameter& Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelGraphParameter>(PropertyHandle);

	const TSharedRef<IPropertyHandle> DefaultValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelGraphParameter, DefaultValue);
	for (const auto& It : Parameter.MetaData)
	{
		DefaultValueHandle->SetInstanceMetaData(It.Key, It.Value);
	}

	DefaultValueCategory.AddProperty(DefaultValueHandle);
}