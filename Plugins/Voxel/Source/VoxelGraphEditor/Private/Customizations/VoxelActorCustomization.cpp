// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"
#include "VoxelActor.h"
#include "IDetailGroup.h"
#include "Sculpt/VoxelSculptStorage.h"
#include "SculptVolume/VoxelSculptEdMode.h"
#include "Customizations/VoxelParameterContainerDetails.h"

VOXEL_CUSTOMIZE_CLASS(AVoxelActor)(IDetailLayoutBuilder& DetailLayout)
{
	if (DetailLayout.GetBaseClass()->IsChildOf<AVoxelPreviewActor>())
	{
		return;
	}

	FVoxelEditorUtilities::EnableRealtime();

	{
		TArray<TWeakObjectPtr<UObject>> Objects;
		DetailLayout.GetObjectsBeingCustomized(Objects);

		DetailLayout.EditCategory(STATIC_FNAME("Sculpt"))
		.AddCustomRow(INVTEXT("Sculpt"))
		.NameContent()
		[
			SNew(SVoxelDetailText)
			.Text(INVTEXT("Clear Sculpt Data"))
		]
		.ValueContent()
		[
			SNew(SVoxelDetailButton)
			.Text(INVTEXT("Clear"))
			.ToolTipText(INVTEXT("Clear all the sculpt data stored in this actor"))
			.OnClicked_Lambda([=]
			{
				FScopedTransaction Transaction(TEXT("Clear Sculpt Data"), INVTEXT("Clear Sculpt Data"), nullptr);

				for (const TWeakObjectPtr<UObject>& Object : Objects)
				{
					if (const AVoxelActor* VoxelActor = Cast<AVoxelActor>(Object.Get()))
					{
						if (ensure(VoxelActor->SculptStorageComponent))
						{
							VoxelActor->SculptStorageComponent->ClearData();
						}
					}
				}

				return FReply::Handled();
			})
		];
	}

	IDetailCategoryBuilder& ActorCategory = DetailLayout.EditCategory(STATIC_FNAME("Actor"));

	TArray<TSharedRef<IPropertyHandle>> ActorProperties;
	ActorCategory.GetDefaultProperties(ActorProperties);

	for (const TSharedRef<IPropertyHandle>& ActorPropertyHandle : ActorProperties)
	{
		if (ActorPropertyHandle->GetProperty()->GetFName() == GET_MEMBER_NAME_STATIC(AActor, Tags))
		{
			IDetailCategoryBuilder& DefaultCategory = DetailLayout.EditCategory(STATIC_FNAME("Default"));
			DefaultCategory.AddProperty(ActorPropertyHandle);
			break;
		}
	}

	DetailLayout.HideCategory(STATIC_FNAME("Actor"));

	IDetailCategoryBuilder& WorldPartitionCategory = DetailLayout.EditCategory(STATIC_FNAME("WorldPartition"));
	TArray<TSharedRef<IPropertyHandle>> WorldPartitionProperties;
	WorldPartitionCategory.GetDefaultProperties(WorldPartitionProperties);

	IDetailCategoryBuilder& DefaultCategory = DetailLayout.EditCategory(STATIC_FNAME("Default"));
	IDetailGroup& Group = DefaultCategory.AddGroup(STATIC_FNAME("WorldPartition"), INVTEXT("World Partition"));
	for (const TSharedRef<IPropertyHandle>& WorldPartitionProperty : WorldPartitionProperties)
	{
		Group.AddPropertyRow(WorldPartitionProperty);
	}

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

	KeepAlive(FVoxelParameterContainerDetails::Create(DetailLayout, PropertyHandle.ToSharedRef()));
}