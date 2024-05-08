// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "Customizations/VoxelPinValueCustomizationHelper.h"

VOXEL_CUSTOMIZE_STRUCT_CHILDREN(FVoxelParameter)(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const TArray<TWeakObjectPtr<UObject>> SelectedObjects = CustomizationUtils.GetPropertyUtilities()->GetSelectedObjects();

	ChildBuilder.AddProperty(PropertyHandle->GetChildHandleStatic(FVoxelParameter, Name));

	const TSharedRef<IPropertyHandle> TypeHandle = PropertyHandle->GetChildHandleStatic(FVoxelParameter, Type);
	if (PropertyHandle->HasMetaData(STATIC_FNAME("FilterTypes")))
	{
		TypeHandle->SetInstanceMetaData(STATIC_FNAME("FilterTypes"), PropertyHandle->GetMetaData(STATIC_FNAME("FilterTypes")));
	}
	ChildBuilder.AddProperty(TypeHandle);

	FString CategoriesList;
	if (const FString* InstanceCategoriesList = PropertyHandle->GetInstanceMetaData("CategoriesList"))
	{
		CategoriesList = *InstanceCategoriesList;
	}
	else
	{
		CategoriesList = PropertyHandle->GetMetaData("CategoriesList");
	}

	// Create category selection
	{
		const TSharedRef<IPropertyHandle> CategoryHandle = PropertyHandle->GetChildHandleStatic(FVoxelParameter, Category);

		if (CategoriesList.IsEmpty())
		{
			ChildBuilder.AddProperty(CategoryHandle);
		}
		else
		{
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

						for (TWeakObjectPtr<UObject> Object : SelectedObjects)
						{
							if (!ensure(Object.IsValid()))
							{
								continue;
							}

							UFunction* Function = Object->FindFunction(*CategoriesList);
							if (!ensure(Function) ||
								!ensure(!Function->Children) ||
								!ensure(Function->ParmsSize == sizeof(Categories)))
							{
								continue;
							}

							Object->ProcessEvent(Function, &Categories);
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
	}

	ChildBuilder.AddProperty(PropertyHandle->GetChildHandleStatic(FVoxelParameter, Description));

	IDetailCategoryBuilder& DefaultValueCategory = ChildBuilder.GetParentCategory().GetParentLayout().EditCategory("Default Value", INVTEXT("Default Value"));

	const auto GetMetaData = [](const TSharedPtr<IPropertyHandle>& Handle)
	{
		const FVoxelParameter Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelParameter>(Handle);
		return Parameter.MetaData;
	};
	const auto SetMetaData = [](const TSharedPtr<IPropertyHandle>& Handle, const TMap<FName, FString>& MetaData)
	{
		FVoxelParameter Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelParameter>(Handle);
		Parameter.MetaData = MetaData;
		FVoxelEditorUtilities::SetStructPropertyValue<FVoxelParameter>(Handle, Parameter);
	};

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

	const FVoxelParameter Parameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelParameter>(PropertyHandle);
	const TSharedRef<IPropertyHandle> DefaultValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelParameter, DefaultValue);
	for (const auto& It : Parameter.MetaData)
	{
		DefaultValueHandle->SetInstanceMetaData(It.Key, It.Value);
	}
	DefaultValueCategory.AddProperty(DefaultValueHandle);
}