// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelOverridableSettingsCustomization.h"

void FVoxelOverridableSettingsCategoryBuilder::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	NodeRow
	.NameContent()
	[
		SNew(SVoxelDetailText)
		.Text(FText::FromString(FEditorCategoryUtils::GetCategoryDisplayString(CategoryName.ToString())))
		.Font(FAppStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
	];
}

void FVoxelOverridableSettingsCategoryBuilder::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	const TSharedPtr<FVoxelOverridableSettingsCustomization> Customization = WeakCustomization.Pin();
	if (!ensure(Customization))
	{
		return;
	}

	for (const TSharedPtr<IPropertyHandle>& Handle : Handles)
	{
		IDetailPropertyRow& PropertyRow = ChildrenBuilder.AddProperty(Handle.ToSharedRef());

		const bool bOverridableHandle = Handle->HasMetaData("Overridable");
		if (!bOverridableHandle ||
			!bIsOverridableStruct)
		{
			continue;
		}

		Customization->SetupHandleEditCondition(PropertyRow, Handle);
	}
}

void FVoxelOverridableSettingsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	OverridableParametersHandle = PropertyHandle->GetChildHandleStatic(FVoxelOverridableSettings, OverridableParameters);

	const bool bIsOverridableStruct = PropertyHandle->HasMetaData("Override");

	uint32 ChildHandlesCount = 0;
	ensure(PropertyHandle->GetNumChildren(ChildHandlesCount) == FPropertyAccess::Result::Success);

	TMap<FName, TSharedRef<FVoxelOverridableSettingsCategoryBuilder>> Categories;

	for (uint32 Index = 0; Index < ChildHandlesCount; Index++)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = PropertyHandle->GetChildHandle(Index);
		if (!ChildHandle)
		{
			continue;
		}

		if (ChildHandle->GetProperty()->GetName() == GET_MEMBER_NAME_STRING_CHECKED(FVoxelOverridableSettings, OverridableParameters))
		{
			continue;
		}

		if (ChildHandle->HasMetaData("ShowOverride"))
		{
			FString ShowOverrideValue = ChildHandle->GetMetaData("ShowOverride");
			if (!bIsOverridableStruct &&
				ShowOverrideValue == "IfOverriden")
			{
				continue;
			}

			if (bIsOverridableStruct &&
				ShowOverrideValue == "IfGlobal")
			{
				continue;
			}

			ensureMsgf(ShowOverrideValue == "IfOverriden" || ShowOverrideValue == "IfGlobal", TEXT("Invalid ShowOverride value: %s"), *ShowOverrideValue);
		}

		FName Category = ChildHandle->GetDefaultCategoryName();
		if (!Category.IsNone() &&
			Category != "default")
		{
			if (!Categories.Contains(Category))
			{
				TSharedRef<FVoxelOverridableSettingsCategoryBuilder> CustomBuilder = MakeVoxelShared<FVoxelOverridableSettingsCategoryBuilder>();
				CustomBuilder->CategoryName = Category;
				CustomBuilder->bIsOverridableStruct = bIsOverridableStruct;
				CustomBuilder->WeakCustomization = SharedThis(this);

				Categories.Add(Category, CustomBuilder);
				ChildBuilder.AddCustomBuilder(CustomBuilder);
			}

			Categories[Category]->Handles.Add(ChildHandle);
			continue;
		}

		IDetailPropertyRow& PropertyRow = ChildBuilder.AddProperty(ChildHandle.ToSharedRef());

		const bool bOverridableHandle = ChildHandle->HasMetaData("Overridable");
		if (!bOverridableHandle ||
			!bIsOverridableStruct)
		{
			continue;
		}

		SetupHandleEditCondition(PropertyRow, ChildHandle);
	}
}

void FVoxelOverridableSettingsCustomization::SetupHandleEditCondition(IDetailPropertyRow& PropertyRow, const TSharedPtr<IPropertyHandle>& Handle) const
{
	SVoxelEditCondition::SetupEditCondition(
		PropertyRow,
		Handle,
		SNew(SVoxelEditCondition)
		.CanEdit_Lambda([this, Handle]
		{
			ECheckBoxState State = ECheckBoxState::Undetermined;
			bool bStateInitialized = false;
			const FName PropertyName = Handle->GetProperty()->GetFName();

			FVoxelEditorUtilities::ForeachData<TSet<FName>>(OverridableParametersHandle, [&](const TSet<FName>& OverridableParameters)
			{
				if (bStateInitialized &&
					State == ECheckBoxState::Undetermined)
				{
					return;
				}
				if (OverridableParameters.Contains(PropertyName))
				{
					if (bStateInitialized &&
						State == ECheckBoxState::Unchecked)
					{
						State = ECheckBoxState::Undetermined;
						return;
					}
					else
					{
						State = ECheckBoxState::Checked;
					}
				}
				else
				{
					if (bStateInitialized &&
						State == ECheckBoxState::Checked)
					{
						State = ECheckBoxState::Undetermined;
						return;
					}
					else
					{
						State = ECheckBoxState::Unchecked;
					}
				}

				bStateInitialized = true;
			});

			return State;
		})
		.OnEditConditionChanged_Lambda([this, Handle](ECheckBoxState NewState)
		{
			TArray<FString> Values;
			OverridableParametersHandle->GetPerObjectValues(Values);

			const FName PropertyName = Handle->GetProperty()->GetFName();
			for (int32 ObjectIndex = 0; ObjectIndex < Values.Num(); ObjectIndex++)
			{
				TSet<FName> OverridableParameters;
				if (FVoxelObjectUtilities::PropertyFromText_Direct(*OverridableParametersHandle->GetProperty(), Values[ObjectIndex], reinterpret_cast<void*>(&OverridableParameters), nullptr))
				{
					if (NewState == ECheckBoxState::Checked)
					{
						OverridableParameters.Add(PropertyName);
					}
					else if (NewState == ECheckBoxState::Unchecked)
					{
						OverridableParameters.Remove(PropertyName);
					}
					else
					{
						ensure(false);
					}

					FString Result = FVoxelObjectUtilities::PropertyToText_Direct(*OverridableParametersHandle->GetProperty(), reinterpret_cast<void*>(&OverridableParameters), nullptr);
					OverridableParametersHandle->SetPerObjectValue(ObjectIndex, Result);
				}
			}
		}));
}

DEFINE_VOXEL_STRUCT_LAYOUT_RECURSIVE_IDENTIFIER(FVoxelOverridableSettings, FVoxelOverridableSettingsCustomization, FVoxelOverridableSettingsIdentifier);
