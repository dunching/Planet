// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "Widgets/SVoxelGraphPinTypeComboBox.h"

class FVoxelPinTypeCustomization : public IPropertyTypeCustomization
{
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		const TArray<TWeakObjectPtr<UObject>> SelectedObjects = CustomizationUtils.GetPropertyUtilities()->GetSelectedObjects();
		FString FilterTypes;
		if (const FString* InstanceFilterTypes = PropertyHandle->GetInstanceMetaData("FilterTypes"))
		{
			FilterTypes = *InstanceFilterTypes;
		}
		else
		{
			FilterTypes = PropertyHandle->GetMetaData("FilterTypes");
		}

		CurrentPinType = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelPinType>(PropertyHandle);

		PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FVoxelPinTypeCustomization::UpdatePinType, MakeWeakPtr(PropertyHandle)));

		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SVoxelPinTypeComboBox)
			.AllowedTypes_Lambda([FilterTypes, SelectedObjects]
			{
				if (FilterTypes.IsEmpty())
				{
					return FVoxelPinTypeSet::All();
				}
				else if (FilterTypes == "AllUniforms")
				{
					return FVoxelPinTypeSet::AllUniforms();
				}
				else if (FilterTypes == "AllBuffers")
				{
					return FVoxelPinTypeSet::AllBuffers();
				}
				else if (FilterTypes == "AllUniformsAndBufferArrays")
				{
					return FVoxelPinTypeSet::AllUniformsAndBufferArrays();
				}
				else
				{
					FVoxelPinTypeSet PinTypes = FVoxelPinTypeSet::All();
					for (TWeakObjectPtr<UObject> Object : SelectedObjects)
					{
						if (!ensure(Object.IsValid()))
						{
							continue;
						}

						UFunction* Function = Object->FindFunction(*FilterTypes);
						if (!ensure(Function) ||
							!ensure(!Function->Children) ||
							!ensure(Function->ParmsSize == sizeof(PinTypes)))
						{
							continue;
						}

						Object->ProcessEvent(Function, &PinTypes);
					}
					return PinTypes;
				}
			})
			.OnTypeChanged_Lambda([=](const FVoxelPinType& Type)
			{
				FVoxelEditorUtilities::SetStructPropertyValue(PropertyHandle, Type);
			})
			.CurrentType_Lambda([this]
			{
				return CurrentPinType;
			})
		];
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
	}

private:
	void UpdatePinType(const TWeakPtr<IPropertyHandle> WeakHandle)
	{
		const TSharedPtr<IPropertyHandle> Handle = WeakHandle.Pin();
		if (!Handle)
		{
			return;
		}

		CurrentPinType = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelPinType>(Handle);
	}

private:
	FVoxelPinType CurrentPinType;
};

DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelPinType, FVoxelPinTypeCustomization);