// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelParameterView.h"

class FVoxelParameterChildBuilder;
class FVoxelParameterContainerDetails;

class FVoxelParameterDetails : public TSharedFromThis<FVoxelParameterDetails>
{
public:
	FVoxelParameterContainerDetails& ContainerDetails;
	const FVoxelParameterPath Path;
	const TVoxelArray<IVoxelParameterView*> ParameterViews;
	FName OrphanName;
	FVoxelPinType OrphanExposedType;

	bool bIsInlineGraph = false;
	FVoxelPinType RowExposedType;

	FVoxelParameterDetails(
		FVoxelParameterContainerDetails& ContainerDetail,
		const FVoxelParameterPath& Path,
		const TVoxelArray<IVoxelParameterView*>& ParameterViews);

	void InitializeOrphan(
		const FVoxelPinValue& Value,
		bool bNewHasSingleValue);

public:
	bool IsValid() const
	{
		return
			ensure(PropertyHandle) &&
			PropertyHandle->IsValidHandle();
	}
	bool IsOrphan() const
	{
		return ParameterViews.Num() == 0;
	}
	bool HasSingleValue() const
	{
		return bHasSingleValue;
	}

public:
	void Tick();

public:
	bool ShouldRebuildChildren() const;
	void RebuildChildren() const;

public:
	void MakeRow(const FVoxelDetailInterface& DetailInterface);

	void BuildRow(
		FDetailWidgetRow& Row,
		const TSharedRef<SWidget>& ValueWidget);

public:
	ECheckBoxState IsEnabled() const;
	void SetEnabled(bool bNewEnabled) const;

public:
	bool CanResetToDefault() const;
	void ResetToDefault();

public:
	void PreEditChange() const;
	void PostEditChange() const;

private:
	const TSharedRef<TStructOnScope<FVoxelPinValue>> StructOnScope = MakeVoxelShared<TStructOnScope<FVoxelPinValue>>();

	double LastSyncTime = 0.;
	bool bHasSingleValue = false;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<FVoxelParameterChildBuilder> ChildBuilder;
	TSharedPtr<FVoxelStructCustomizationWrapper> StructWrapper;

	void SyncFromViews();
	FVoxelPinValue& GetValueRef() const;
};