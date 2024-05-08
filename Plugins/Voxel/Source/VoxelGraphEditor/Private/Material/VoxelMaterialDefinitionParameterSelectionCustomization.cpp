// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMaterialDefinitionParameterSelectionCustomization.h"
#include "Material/VoxelMaterialDefinition.h"

void FVoxelMaterialDefinitionParameterSelectionCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const TArray<TWeakObjectPtr<UObject>> SelectedObjects = DetailLayout.GetSelectedObjects();
	if (SelectedObjects.Num() != 1)
	{
		return;
	}

	UVoxelMaterialDefinition* MaterialDefinition = Cast<UVoxelMaterialDefinition>(SelectedObjects[0].Get());
	if (!ensure(MaterialDefinition))
	{
		return;
	}

	DetailLayout.HideCategory("Config");

	{
		const TSharedRef<IPropertyHandle> ParametersHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelMaterialDefinition, Parameters), UVoxelMaterialDefinition::StaticClass());
		DetailLayout.HideProperty(ParametersHandle);

		uint32 ParametersCount = 0;
		ParametersHandle->GetNumChildren(ParametersCount);

		TSharedPtr<IPropertyHandle> ParameterHandle;
		for (uint32 Index = 0; Index < ParametersCount; Index++)
		{
			const TSharedPtr<IPropertyHandle> ChildParameterHandle = ParametersHandle->GetChildHandle(Index);

			FVoxelParameter ChildParameter = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelParameter>(ChildParameterHandle);
			if (ChildParameter.Guid != TargetParameterId)
			{
				continue;
			}

			ensure(!ParameterHandle);
			ParameterHandle = ChildParameterHandle;
		}

		if (!ensure(ParameterHandle))
		{
			return;
		}

		IDetailCategoryBuilder& Builder = DetailLayout.EditCategory("Parameter", INVTEXT("Parameter"));
		Builder.AddProperty(ParameterHandle);
	}

	{
		const TSharedRef<IPropertyHandle> GuidToParameterDataHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelMaterialDefinition, GuidToParameterData), UVoxelMaterialDefinition::StaticClass());
		DetailLayout.HideProperty(GuidToParameterDataHandle);

		uint32 ParametersCount = 0;
		GuidToParameterDataHandle->GetNumChildren(ParametersCount);

		TSharedPtr<IPropertyHandle> ParameterDataHandle;
		for (uint32 Index = 0; Index < ParametersCount; Index++)
		{
			const TSharedPtr<IPropertyHandle> ValueHandle = GuidToParameterDataHandle->GetChildHandle(Index);
			if (!ensure(ValueHandle))
			{
				continue;
			}
			const TSharedPtr<IPropertyHandle> KeyHandle = ValueHandle->GetKeyHandle();
			if (!ensure(KeyHandle))
			{
				continue;
			}

			if (FVoxelEditorUtilities::GetStructPropertyValue<FGuid>(KeyHandle) != TargetParameterId)
			{
				continue;
			}

			ensure(!ParameterDataHandle);
			ParameterDataHandle = ValueHandle;
		}

		if (!ParameterDataHandle)
		{
			return;
		}

		Wrapper = FVoxelStructCustomizationWrapper::Make(ParameterDataHandle.ToSharedRef());
		if (!Wrapper)
		{
			return;
		}

		IDetailCategoryBuilder& Builder = DetailLayout.EditCategory("Parameter Data", INVTEXT("Parameter Data"));
		for (const TSharedPtr<IPropertyHandle>& Handle : Wrapper->AddChildStructure())
		{
			Builder.AddProperty(Handle);
		}
	}
}