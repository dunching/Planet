// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelChannelEditorDefinitionCustomization.h"
#include "VoxelSettings.h"
#include "Widgets/SVoxelChannelEditor.h"

void FVoxelChannelEditorDefinitionCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const TSharedRef<IPropertyHandle> EditChannelHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(FVoxelChannelEditorDefinition, bEditChannel), FVoxelChannelEditorDefinition::StaticStruct());
	const TSharedRef<IPropertyHandle> NameHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(FVoxelChannelExposedDefinition, Name), FVoxelChannelExposedDefinition::StaticStruct());
	const TSharedRef<IPropertyHandle> TypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(FVoxelChannelExposedDefinition, Type), FVoxelChannelExposedDefinition::StaticStruct());
	const TSharedRef<IPropertyHandle> DefaultValueHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(FVoxelChannelExposedDefinition, DefaultValue), FVoxelChannelExposedDefinition::StaticStruct());

	const auto EditConditionLambda = MakeAttributeLambda([EditChannelHandle]
	{
		bool bEditChannel = false;
		EditChannelHandle->GetValue(bEditChannel);
		return !bEditChannel;
	});

	DetailLayout.AddPropertyToCategory(NameHandle).EditCondition(EditConditionLambda, nullptr);

	DetailLayout.AddPropertyToCategory(TypeHandle);
	DetailLayout.AddPropertyToCategory(DefaultValueHandle);
	DetailLayout.HideProperty(EditChannelHandle);

	const TSharedRef<IPropertyHandle> SaveLocationHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(FVoxelChannelEditorDefinition, SaveLocation), FVoxelChannelEditorDefinition::StaticStruct());
	if (!FVoxelEditorUtilities::IsSingleValue(SaveLocationHandle))
	{
		return;
	}

	DetailLayout.AddCustomRowToCategory(SaveLocationHandle, INVTEXT("Save Location"))
	.EditCondition(EditConditionLambda, nullptr)
	.NameContent()
	[
		SaveLocationHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SBox)
		.MinDesiredWidth(125.f)
		[
			SNew(SVoxelDetailComboBox<TWeakObjectPtr<const UObject>>)
			.RefreshDelegate(SaveLocationHandle, DetailLayout)
			.Options_Lambda([]
			{
				TArray<TWeakObjectPtr<const UObject>> Result;
				Result.Add(UVoxelSettings::StaticClass()->GetDefaultObject());

				for (const UObject* Asset : GVoxelChannelManager->GetChannelAssets())
				{
					if (!Cast<UVoxelSettings>(Asset))
					{
						Result.Add(Asset);
					}
				}

				return Result;
			})
			.CurrentOption(FVoxelEditorUtilities::GetUObjectProperty<UObject>(SaveLocationHandle))
			.OptionText(MakeLambdaDelegate([](const TWeakObjectPtr<const UObject> Option) -> FString
			{
				if (Cast<const UVoxelSettings>(Option))
				{
					return "Project";
				}

				return Option.IsValid() ? Option->GetName() : "INVALID";
			}))
			.OnSelection_Lambda([SaveLocationHandle](const TWeakObjectPtr<const UObject> NewValue)
			{
				ensure(SaveLocationHandle->SetValue(NewValue.Get()) == FPropertyAccess::Result::Success);
			})
		]
	];
}