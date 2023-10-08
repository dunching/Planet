// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelInlineGraph.h"
#include "Customizations/VoxelParameterContainerDetails.h"

class FVoxelInlineGraphCustomization : public FVoxelPropertyTypeCustomization
{
public:
	TWeakPtr<SBox> WeakBox;

	virtual void CustomizeHeader(
		const TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SAssignNew(WeakBox, SBox)
		];
	}
	virtual void CustomizeChildren(
		const TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		const TSharedPtr<SBox> Box = WeakBox.Pin();
		if (!ensure(Box))
		{
			return;
		}

		const TSharedRef<IPropertyHandle> ParameterContainerHandle = PropertyHandle->GetChildHandleStatic(FVoxelInlineGraph, ParameterContainer);
		KeepAlive(FVoxelParameterContainerDetails::Create(
			ChildBuilder,
			ParameterContainerHandle,
			Box));
	}
};

DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelInlineGraph, FVoxelInlineGraphCustomization);