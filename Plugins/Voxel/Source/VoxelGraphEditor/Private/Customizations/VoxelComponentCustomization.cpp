// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelComponent.h"
#include "Customizations/VoxelParameterContainerDetails.h"

VOXEL_CUSTOMIZE_CLASS(UVoxelComponent)(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailLayout.GetObjectsBeingCustomized(WeakObjects);

	TArray<UObject*> Objects;
	for (const TWeakObjectPtr<UObject>& WeakObject : WeakObjects)
	{
		Objects.Add(WeakObject.Get());
	}

	IDetailPropertyRow* Row = DetailLayout.EditCategory("").AddExternalObjectProperty(
		Objects,
		GET_MEMBER_NAME_STATIC(UVoxelComponent, ParameterContainer),
		EPropertyLocation::Default,
		FAddPropertyParams().ForceShowProperty());

	if (!ensure(Row))
	{
		return;
	}

	Row->Visibility(EVisibility::Collapsed);

	const TSharedPtr<IPropertyHandle> PropertyHandle = Row->GetPropertyHandle();
	if (!ensure(PropertyHandle))
	{
		return;
	}

	KeepAlive(FVoxelParameterContainerDetails::Create(DetailLayout, PropertyHandle.ToSharedRef()));
}