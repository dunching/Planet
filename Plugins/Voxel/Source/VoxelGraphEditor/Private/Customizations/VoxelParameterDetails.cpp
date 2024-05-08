// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Customizations/VoxelParameterDetails.h"
#include "Customizations/VoxelParameterChildBuilder.h"
#include "Customizations/VoxelParameterContainerDetails.h"
#include "Customizations/VoxelPinValueCustomizationHelper.h"
#include "VoxelParameterContainer.h"

FVoxelParameterDetails::FVoxelParameterDetails(
	FVoxelParameterContainerDetails& ContainerDetail,
	const FVoxelParameterPath& Path,
	const TVoxelArray<IVoxelParameterView*>& ParameterViews)
	: ContainerDetails(ContainerDetail)
	, Path(Path)
	, ParameterViews(ParameterViews)
{
	StructOnScope->InitializeAs<FVoxelPinValue>();

	if (!IsOrphan())
	{
		SyncFromViews();
	}
}

void FVoxelParameterDetails::InitializeOrphan(
	const FVoxelPinValue& Value,
	const bool bNewHasSingleValue)
{
	ensure(IsOrphan());
	ensure(Value.IsValid());

	GetValueRef() = Value;
	bHasSingleValue = bNewHasSingleValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterDetails::Tick()
{
	const double Time = FPlatformTime::Seconds();
	if (!IsOrphan() &&
		LastSyncTime + 0.1 < Time)
	{
		LastSyncTime = Time;
		SyncFromViews();
	}

	if (ChildBuilder)
	{
		ChildBuilder->UpdateExpandedState();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelParameterDetails::ShouldRebuildChildren() const
{
	if (!ChildBuilder)
	{
		return false;
	}

	if (ChildBuilder->ParameterViewsCommonChildren.Num() == 0)
	{
		// Not built yet
		return false;
	}

	return
		ChildBuilder->ParameterViewsCommonChildren !=
		IVoxelParameterViewBase::GetCommonChildren(ParameterViews);
}

void FVoxelParameterDetails::RebuildChildren() const
{
	if (!ensure(ChildBuilder))
	{
		return;
	}

	ensure(ChildBuilder->OnRegenerateChildren.ExecuteIfBound());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterDetails::MakeRow(const FVoxelDetailInterface& DetailInterface)
{
	VOXEL_FUNCTION_COUNTER();

	if (!IsOrphan())
	{
		ensure(!RowExposedType.IsValid());
		RowExposedType = ParameterViews[0]->GetType().GetExposedType();
	}
	else
	{
		RowExposedType = OrphanExposedType;
	}

	IDetailPropertyRow* DummyRow = DetailInterface.AddExternalStructure(StructOnScope);
	if (!ensure(DummyRow))
	{
		return;
	}

	DummyRow->Visibility(EVisibility::Collapsed);

	ensure(!PropertyHandle);
	PropertyHandle = DummyRow->GetPropertyHandle();

	if (!ensure(PropertyHandle))
	{
		return;
	}

	FVoxelEditorUtilities::TrackHandle(PropertyHandle);

	const FSimpleDelegate PreChangeDelegate = MakeWeakPtrDelegate(this, [this]
	{
		PreEditChange();
	});
	const FSimpleDelegate PostChangeDelegate = MakeWeakPtrDelegate(this, [this]
	{
		PostEditChange();
	});

	PropertyHandle->SetOnPropertyValuePreChange(PreChangeDelegate);
	PropertyHandle->SetOnPropertyValueChanged(PostChangeDelegate);

	PropertyHandle->SetOnChildPropertyValuePreChange(PreChangeDelegate);
	PropertyHandle->SetOnChildPropertyValueChanged(PostChangeDelegate);

	ensure(!bIsInlineGraph);

	for (const IVoxelParameterView* ParameterView : ParameterViews)
	{
		if (ParameterView->IsInlineGraph())
		{
			bIsInlineGraph = true;
		}
	}

	for (const IVoxelParameterView* ParameterView : ParameterViews)
	{
		ensure(ParameterView->IsInlineGraph() == bIsInlineGraph);
	}

	if (bIsInlineGraph)
	{
		// InlineGraph Graph object
		const TSharedRef<IPropertyHandle> ObjectHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Object);

		ObjectHandle->GetProperty()->SetMetaData("AllowedClasses", UVoxelGraphInterface::StaticClass()->GetPathName());
		const TSharedRef<SWidget> ValueWidget = ObjectHandle->CreatePropertyValueWidget();
		ObjectHandle->GetProperty()->RemoveMetaData("AllowedClasses");

		ensure(!ChildBuilder);
		ChildBuilder = MakeVoxelShared<FVoxelParameterChildBuilder>(*this, ValueWidget);

		// Rebuild children when changing graph
		ObjectHandle->SetOnPropertyValueChanged(MakeWeakPtrDelegate(ChildBuilder, [&ChildBuilderRef = *ChildBuilder]
		{
			// TODO Fire generic OnChanged and diff layout + rebuild if needed
			(void)ChildBuilderRef.OnRegenerateChildren.ExecuteIfBound();
		}));

		DetailInterface.AddCustomBuilder(ChildBuilder.ToSharedRef());
		return;
	}

	bool bMetadataSet = false;
	TMap<FName, FString> MetaData;
	for (const IVoxelParameterView* ParameterView : ParameterViews)
	{
		TMap<FName, FString> NewMetaData = ParameterView->GetMetaData();
		if (bMetadataSet)
		{
			ensure(MetaData.OrderIndependentCompareEqual(NewMetaData));
		}
		else
		{
			bMetadataSet = true;
			MetaData = MoveTemp(NewMetaData);
		}
	}

	ensure(!StructWrapper);
	StructWrapper = FVoxelPinValueCustomizationHelper::CreatePinValueCustomization(
		PropertyHandle.ToSharedRef(),
		DetailInterface,
		MetaData,
		[&](FDetailWidgetRow& Row, const TSharedRef<SWidget>& ValueWidget)
		{
			BuildRow(Row, ValueWidget);
		},
		// Used to load/save expansion state
		FAddPropertyParams().UniqueId(FName(Path.ToString())),
		MakeAttributeLambda(MakeWeakPtrLambda(this, [this]
		{
			return
				ContainerDetails.AlwaysEnabled() ||
				IsOrphan() ||
				IsEnabled() == ECheckBoxState::Checked;
		})));
}

void FVoxelParameterDetails::BuildRow(
	FDetailWidgetRow& Row,
	const TSharedRef<SWidget>& ValueWidget)
{
	VOXEL_FUNCTION_COUNTER();

	FVoxelPinType ExposedType;
	if (ParameterViews.Num() > 0)
	{
		ExposedType = ParameterViews[0]->GetType().GetExposedType();
		for (const IVoxelParameterView* ParameterView : ParameterViews)
		{
			ensure(ExposedType == ParameterView->GetType().GetExposedType());
		}
	}
	else
	{
		ExposedType = OrphanExposedType;
	}

	const float Width = FVoxelPinValueCustomizationHelper::GetValueWidgetWidthByType(PropertyHandle, ExposedType);

	TSharedRef<SWidget> NameWidget =
		SNew(SVoxelDetailText)
		.ColorAndOpacity(IsOrphan() ? FLinearColor::Red : FSlateColor::UseForeground())
		.Text_Lambda(MakeWeakPtrLambda(this, [this]
		{
			if (ParameterViews.Num() == 0)
			{
				return FText::FromName(OrphanName);
			}

			const FName Name = ParameterViews[0]->GetName();
			for (const IVoxelParameterView* ParameterView : ParameterViews)
			{
				ensure(Name == ParameterView->GetName());
			}
			return FText::FromName(Name);
		}));

	if (!ContainerDetails.AlwaysEnabled())
	{
		const TAttribute<bool> EnabledAttribute = MakeAttributeLambda(MakeWeakPtrLambda(this, [this]
		{
			return
				IsOrphan() ||
				IsEnabled() == ECheckBoxState::Checked;
		}));

		Row.IsEnabled(EnabledAttribute);
		NameWidget->SetEnabled(EnabledAttribute);
		ValueWidget->SetEnabled(EnabledAttribute);

		NameWidget =
			SNew(SVoxelAlwaysEnabledWidget)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked_Lambda(MakeWeakPtrLambda(this, [this]
					{
						return IsEnabled();
					}))
					.OnCheckStateChanged_Lambda(MakeWeakPtrLambda(this, [this](const ECheckBoxState NewState)
					{
						ensure(NewState != ECheckBoxState::Undetermined);
						SetEnabled(NewState == ECheckBoxState::Checked);
					}))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.FillWidth(1.f)
				[
					NameWidget
				]
			];
	}

	Row
	.NameContent()
	[
		NameWidget
	]
	.ValueContent()
	.MinDesiredWidth(Width)
	.MaxDesiredWidth(Width)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SVoxelDetailText)
			.Text(INVTEXT("Multiple Values"))
			.Visibility_Lambda(MakeWeakPtrLambda(this, [this]
			{
				return HasSingleValue() ? EVisibility::Collapsed : EVisibility::Visible;
			}))
		]
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.Visibility_Lambda(MakeWeakPtrLambda(this, [this]
			{
				return HasSingleValue() ? EVisibility::Visible : EVisibility::Collapsed;
			}))
			[
				ValueWidget
			]
		]
	]
	.OverrideResetToDefault(FResetToDefaultOverride::Create(
		MakeAttributeLambda(MakeWeakPtrLambda(this, [this]
		{
			return CanResetToDefault();
		})),
		MakeWeakPtrDelegate(this, [this]
		{
			ResetToDefault();
		}),
		false));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ECheckBoxState FVoxelParameterDetails::IsEnabled() const
{
	ensure(!ContainerDetails.AlwaysEnabled());

	bool bAnyEnabled = false;
	bool bAnyDisabled = false;
	for (const UVoxelParameterContainer* ParameterContainer : ContainerDetails.GetParameterContainers())
	{
		if (const FVoxelParameterValueOverride* ValueOverride = ParameterContainer->ValueOverrides.Find(Path))
		{
			if (ValueOverride->bEnable)
			{
				bAnyEnabled = true;
			}
			else
			{
				bAnyDisabled = true;
			}
		}
		else
		{
			bAnyDisabled = true;
		}
	}

	if (bAnyEnabled && !bAnyDisabled)
	{
		return ECheckBoxState::Checked;
	}
	if (!bAnyEnabled && bAnyDisabled)
	{
		return ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Undetermined;
}

void FVoxelParameterDetails::SetEnabled(const bool bNewEnabled) const
{
	ensure(!ContainerDetails.AlwaysEnabled());

	const TVoxelArray<UVoxelParameterContainer*> ParameterContainers = ContainerDetails.GetParameterContainers();
	if (!ensure(ParameterContainers.Num() == ParameterViews.Num()))
	{
		return;
	}

	for (int32 Index = 0; Index < ParameterContainers.Num(); Index++)
	{
		UVoxelParameterContainer* ParameterContainer = ParameterContainers[Index];
		ParameterContainer->PreEditChange(nullptr);

		if (FVoxelParameterValueOverride* ExistingValueOverride = ParameterContainer->ValueOverrides.Find(Path))
		{
			ExistingValueOverride->bEnable = bNewEnabled;
			ensure(ExistingValueOverride->Value.IsValid());
		}
		else
		{
			const IVoxelParameterView* ParameterView = ParameterViews[Index];
			if (!ensure(ParameterView))
			{
				continue;
			}

			FVoxelParameterValueOverride NewValueOverride;
			NewValueOverride.bEnable = true;
			NewValueOverride.CachedName = ParameterView->GetName();
			NewValueOverride.CachedCategory = FName(ParameterView->GetCategory());
			NewValueOverride.Value = ParameterView->GetValue();

			// Add AFTER doing GetValue so we don't query ourselves
			ParameterContainer->ValueOverrides.Add(Path, NewValueOverride);
		}

		ParameterContainer->Fixup();
		ParameterContainer->PostEditChange();
		ParameterContainer->OnChanged.Broadcast();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelParameterDetails::CanResetToDefault() const
{
	if (IsOrphan())
	{
		return true;
	}

	const TVoxelArray<UVoxelParameterContainer*> ParameterContainers = ContainerDetails.GetParameterContainers();
	if (!ensure(ParameterContainers.Num() == ParameterViews.Num()))
	{
		return false;
	}

	for (int32 Index = 0; Index < ParameterContainers.Num(); Index++)
	{
		UVoxelParameterContainer* ParameterContainer = ParameterContainers[Index];
		const IVoxelParameterView* ParameterView = ParameterViews[Index];
		if (!ensure(ParameterView))
		{
			continue;
		}

		const FVoxelParameterValueOverride* ValueOverride = ParameterContainer->ValueOverrides.Find(Path);
		if (!ValueOverride)
		{
			continue;
		}

		ParameterView->RootView.GetContext().AddValueOverrideToIgnore(FVoxelParameterContainerRef::MakeRoot(ParameterContainer), Path);
		const FVoxelPinValue DefaultValue = ParameterView->GetValue();
		ParameterView->RootView.GetContext().RemoveValueOverrideToIgnore(FVoxelParameterContainerRef::MakeRoot(ParameterContainer), Path);

		if (ValueOverride->Value != DefaultValue)
		{
			return true;
		}
	}
	return false;
}

void FVoxelParameterDetails::ResetToDefault()
{
	if (IsOrphan())
	{
		for (UVoxelParameterContainer* ParameterContainer : ContainerDetails.GetParameterContainers())
		{
			ParameterContainer->PreEditChange(nullptr);
			ParameterContainer->ValueOverrides.Remove(Path);
			ParameterContainer->Fixup();
			ParameterContainer->PostEditChange();

			// No need to broadcast OnChanged for orphans
		}

		// Force refresh to remove orphans rows that were removed
		ContainerDetails.ForceRefresh();

		return;
	}

	const TVoxelArray<UVoxelParameterContainer*> ParameterContainers = ContainerDetails.GetParameterContainers();
	if (!ensure(ParameterContainers.Num() == ParameterViews.Num()))
	{
		return;
	}

	for (int32 Index = 0; Index < ParameterContainers.Num(); Index++)
	{
		UVoxelParameterContainer* ParameterContainer = ParameterContainers[Index];
		const IVoxelParameterView* ParameterView = ParameterViews[Index];
		if (!ensure(ParameterView))
		{
			continue;
		}

		FVoxelParameterValueOverride* ValueOverride = ParameterContainer->ValueOverrides.Find(Path);
		if (!ValueOverride)
		{
			// We might be able to only reset to default one of the multi-selected objects
			ensure(ContainerDetails.GetParameterContainers().Num() > 1);
			continue;
		}

		ParameterView->RootView.GetContext().AddValueOverrideToIgnore(FVoxelParameterContainerRef::MakeRoot(ParameterContainer), Path);
		const FVoxelPinValue DefaultValue = ParameterView->GetValue();
		ParameterView->RootView.GetContext().RemoveValueOverrideToIgnore(FVoxelParameterContainerRef::MakeRoot(ParameterContainer), Path);

		ParameterContainer->PreEditChange(nullptr);
		ValueOverride->Value = DefaultValue;
		ParameterContainer->Fixup();
		ParameterContainer->PostEditChange();

		ParameterContainer->OnChanged.Broadcast();

		// Do this now as caller will broadcast PostChangeDelegate
		SyncFromViews();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterDetails::PreEditChange() const
{
	for (UVoxelParameterContainer* ParameterContainer : ContainerDetails.GetParameterContainers())
	{
		ParameterContainer->PreEditChange(nullptr);
	}
}

void FVoxelParameterDetails::PostEditChange() const
{
	for (UVoxelParameterContainer* ParameterContainer : ContainerDetails.GetParameterContainers())
	{
		FVoxelParameterValueOverride& ValueOverride = ParameterContainer->ValueOverrides.FindOrAdd(Path);
		if (ParameterContainer->bAlwaysEnabled)
		{
			ValueOverride.bEnable = true;
		}
		else
		{
			ensure(ValueOverride.bEnable);
		}

		ValueOverride.Value = GetValueRef();
		ParameterContainer->Fixup();
		ParameterContainer->PostEditChange();

		ParameterContainer->OnChanged.Broadcast();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterDetails::SyncFromViews()
{
	ensure(!IsOrphan());

	bHasSingleValue = true;

	bool bValueIsSet = false;
	FVoxelPinValue Value;
	for (const IVoxelParameterView* ParameterView : ParameterViews)
	{
		FVoxelPinValue NewValue = ParameterView->GetValue();
		if (bValueIsSet)
		{
			if (Value != NewValue)
			{
				bHasSingleValue = false;
			}
		}
		else
		{
			bValueIsSet = true;
			Value = MoveTemp(NewValue);
		}
	}
	ensure(bValueIsSet);

	// Always set value to the first view, otherwise we can't get Type from property handle
	GetValueRef() = Value;
}

FVoxelPinValue& FVoxelParameterDetails::GetValueRef() const
{
	FVoxelPinValue* Value = StructOnScope->Get();
	check(Value);
	return *Value;
}