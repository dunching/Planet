// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelSettings.h"

class FVoxelChannelExposedDefinitionCustomization : public IPropertyTypeCustomization
{
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SBox)
			.VAlign(VAlign_Center)
			[
				SNew(SVoxelDetailText)
				.Text_Lambda([=]
				{
					const TSharedRef<IPropertyHandle> Handle = PropertyHandle->GetChildHandleStatic(FVoxelChannelExposedDefinition, Name);

					FName Name;
					Handle->GetValue(Name);
					return FText::FromName(Name);
				})
			]
		];
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		const TSharedRef<IPropertyHandle> NameHandle = PropertyHandle->GetChildHandleStatic(FVoxelChannelExposedDefinition, Name);
		const TSharedRef<IPropertyHandle> TypeHandle = PropertyHandle->GetChildHandleStatic(FVoxelChannelExposedDefinition, Type);
		const TSharedRef<IPropertyHandle> DefaultValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelChannelExposedDefinition, DefaultValue);

		ChildBuilder.AddProperty(NameHandle);
		ChildBuilder.AddProperty(TypeHandle);
		ChildBuilder.AddProperty(DefaultValueHandle);

		TypeHandle->SetOnPropertyValueChanged(FVoxelEditorUtilities::MakeRefreshDelegate(TypeHandle, CustomizationUtils));
	}
};

DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelChannelExposedDefinition, FVoxelChannelExposedDefinitionCustomization);