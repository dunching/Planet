// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "Material/VoxelMaterialDefinition.h"
#include "Material/MaterialExpressionSampleVoxelParameter.h"

class FMaterialExpressionVoxelParameterCustomization : public FVoxelDetailCustomization
{
public:
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override
	{
		IDetailCategoryBuilder& Category = DetailLayout.EditCategory(STATIC_FNAME("Config"));

		const TSharedRef<IPropertyHandle> MaterialDefinitionHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UMaterialExpressionVoxelParameter, MaterialDefinition));
		Category.AddProperty(MaterialDefinitionHandle);
		MaterialDefinitionHandle->SetOnPropertyValueChanged(MakeLambdaDelegate([this]
		{
			UpdateMaterialDefinitionData();
		}));

		const TSharedRef<IPropertyHandle> ParameterGuidHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UMaterialExpressionVoxelParameter, ParameterGuid));
		ParameterGuidHandle->MarkHiddenByCustomization();

		FGuid* CurrentValue = nullptr;
		if (!FVoxelEditorUtilities::GetPropertyValue(ParameterGuidHandle, CurrentValue))
		{
			return;
		}

		Category.AddCustomRow(INVTEXT("Name"))
		.Visibility(MakeAttributeLambda([this]
		{
			return bHasValidDefinition ? EVisibility::Visible : EVisibility::Collapsed;
		}))
		.NameContent()
		[
		ParameterGuidHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SBox)
			.MinDesiredWidth(125.f)
			[
				SAssignNew(ParameterComboBox, SVoxelDetailComboBox<FGuid>)
				.RefreshDelegate(ParameterGuidHandle, DetailLayout)
				.Options_Lambda([=]
				{
					return AvailableParameters;
				})
				.CurrentOption(*CurrentValue)
				.OptionText(MakeLambdaDelegate([this](const FGuid Option) -> FString
				{
					UVoxelMaterialDefinition* MaterialDefinition = WeakMaterialDefinition.Get();
					if (!ensure(MaterialDefinition))
					{
						return {};
					}

					const FVoxelParameter* Parameter = MaterialDefinition->Parameters.FindByKey(Option);
					if (!Parameter)
					{
						return "Invalid";
					}
					return Parameter->Name.ToString();
				}))
				.OnSelection_Lambda([ParameterGuidHandle](const FGuid NewValue)
				{
					FVoxelEditorUtilities::SetStructPropertyValue(ParameterGuidHandle, NewValue);
				})
			]
		];

		SelectedObjects = DetailLayout.GetSelectedObjects();
		WeakParameterGuidHandle = ParameterGuidHandle;
		UpdateMaterialDefinitionData();
	}

	void UpdateMaterialDefinitionData()
	{
		bHasValidDefinition = false;
		AvailableParameters = {};

		static TSet<FVoxelPinType> ValidTypes;
		if (ValidTypes.Num() == 0)
		{
			for (const TSubclassOf<UMaterialExpressionSampleVoxelParameter> Class : GetDerivedClasses<UMaterialExpressionSampleVoxelParameter>())
			{
				if (Class->HasAnyClassFlags(CLASS_Abstract))
				{
					continue;
				}

				ValidTypes.Add(Class.GetDefaultObject()->GetVoxelParameterType());
			}
		}

		TSet<UVoxelMaterialDefinition*> MaterialDefinitions;
		for (const TWeakObjectPtr<UObject>& SelectedObject : SelectedObjects)
		{
			const UMaterialExpressionVoxelParameter* Expression = Cast<UMaterialExpressionVoxelParameter>(SelectedObject.Get());
			if (!ensure(Expression))
			{
				continue;
			}

			MaterialDefinitions.Add(Expression->MaterialDefinition);
		}

		if (MaterialDefinitions.Num() != 1)
		{
			return;
		}

		if (WeakMaterialDefinition.IsValid())
		{
			WeakMaterialDefinition->OnParametersChanged.Remove(ParametersChangeHandle);
		}

		WeakMaterialDefinition = MaterialDefinitions.Array()[0];
		if (!WeakMaterialDefinition.IsValid())
		{
			return;
		}

		ParametersChangeHandle = WeakMaterialDefinition->OnParametersChanged.AddSP(this, &FMaterialExpressionVoxelParameterCustomization::UpdateMaterialDefinitionData);

		const TSharedPtr<IPropertyHandle> ParameterGuidHandle = WeakParameterGuidHandle.Pin();
		if (!ensure(ParameterGuidHandle))
		{
			return;
		}

		FGuid* CurrentValue = nullptr;
		if (!FVoxelEditorUtilities::GetPropertyValue(ParameterGuidHandle, CurrentValue))
		{
			return;
		}

		for (const FVoxelParameter& Parameter : WeakMaterialDefinition->Parameters)
		{
			if (!ValidTypes.Contains(Parameter.Type))
			{
				continue;
			}

			AvailableParameters.Add(Parameter.Guid);
		}

		ParameterComboBox->SetCurrentItem(*CurrentValue);
		ParameterComboBox->RefreshOptionsList();
		bHasValidDefinition = true;
	}

private:
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	TArray<FGuid> AvailableParameters;
	bool bHasValidDefinition = false;

	TSharedPtr<SVoxelDetailComboBox<FGuid>> ParameterComboBox;
	TWeakPtr<IPropertyHandle> WeakParameterGuidHandle;
	TWeakObjectPtr<UVoxelMaterialDefinition> WeakMaterialDefinition;

	FDelegateHandle ParametersChangeHandle;
};

DEFINE_VOXEL_CLASS_LAYOUT(UMaterialExpressionVoxelParameter, FMaterialExpressionVoxelParameterCustomization);