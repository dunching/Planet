// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelActor.h"
#include "SculptVolume/VoxelSculptEdMode.h"
#include "Customizations/VoxelParameterContainerDetails.h"

VOXEL_CUSTOMIZE_CLASS(AVoxelPreviewActor)(IDetailLayoutBuilder& DetailLayout)
{
	FVoxelEditorUtilities::EnableRealtime();

	DetailLayout.AddPropertyToCategory(DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(AVoxelPreviewActor, TargetActor)));

	DetailLayout.HideCategory(STATIC_FNAME("TransformCommon"));
	DetailLayout.HideCategory(STATIC_FNAME("Actor"));
	DetailLayout.HideCategory(STATIC_FNAME("WorldPartition"));
	DetailLayout.HideCategory(STATIC_FNAME("LevelInstance"));

	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailLayout.GetObjectsBeingCustomized(WeakObjects);

	TArray<UObject*> Objects;
	for (const TWeakObjectPtr<UObject>& WeakObject : WeakObjects)
	{
		Objects.Add(WeakObject.Get());
	}

	IDetailPropertyRow* Row = DetailLayout.EditCategory("").AddExternalObjectProperty(
		Objects,
		GET_MEMBER_NAME_STATIC(AVoxelActor, ParameterContainer),
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

	PropertyHandle->SetInstanceMetaData("HideProvider", "");
	KeepAlive(FVoxelParameterContainerDetails::Create(DetailLayout, PropertyHandle.ToSharedRef()));
}