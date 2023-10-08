// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelParameterView.h"

class FVoxelParameterDetails;
class UVoxelParameterContainer;

class VOXELGRAPHEDITOR_API FVoxelParameterContainerDetails
	: public FVoxelTicker
	, public TSharedFromThis<FVoxelParameterContainerDetails>
{
public:
	static TSharedPtr<FVoxelParameterContainerDetails> Create(
		const FVoxelDetailCategoryInterface& DetailCategoryInterface,
		const TSharedRef<IPropertyHandle>& ParameterContainerHandle,
		const TSharedPtr<SBox>& ProviderDropdownContainer = nullptr);

	VOXEL_COUNT_INSTANCES();

	//~ Begin FVoxelTicker Interface
	virtual void Tick() override;
	//~ End FVoxelTicker Interface

public:
	TVoxelArray<UVoxelParameterContainer*> GetParameterContainers() const;

	bool AlwaysEnabled() const
	{
		return bAlwaysEnabled;
	}

	void ForceRefresh() const
	{
		ensure(RefreshDelegate.ExecuteIfBound());
	}

	void GenerateView(
		const TVoxelArray<IVoxelParameterView*>& ParameterViews,
		const FVoxelDetailInterface& DetailInterface);

	void AddOrphans(
		const FVoxelParameterPath& BasePath,
		TConstVoxelArrayView<IVoxelParameterViewBase*> ParameterViews,
		const FVoxelDetailInterface& DetailInterface,
		FName Category);

private:
	const TSharedRef<IPropertyHandle> ParameterContainerHandle;
	TVoxelArray<TSharedRef<IVoxelParameterRootView>> SharedParameterRootViews;
	TVoxelArray<IVoxelParameterViewBase*> ParameterRootViews;
	TVoxelArray<TWeakObjectPtr<UVoxelParameterContainer>> WeakParameterContainers;
	TVoxelArray<TVoxelArray<IVoxelParameterView*>> ParameterRootViewsCommonChildren;
	bool bAlwaysEnabled = false;

	TOptional<TArray<FString>> LastCategories;
	FSimpleDelegate RefreshDelegate;

	bool bRefreshQueued = false;
	TVoxelMap<FVoxelParameterPath, TSharedPtr<FVoxelParameterDetails>> PathToParameterDetails;

private:
	explicit FVoxelParameterContainerDetails(const TSharedRef<IPropertyHandle>& ParameterContainerHandle)
		: ParameterContainerHandle(ParameterContainerHandle)
	{
	}

	void Initialize();
	void Initialize(
		const FVoxelDetailCategoryInterface& DetailCategoryInterface,
		const TSharedPtr<SBox>& ProviderDropdownContainer);

	void AddParameters(
		const FVoxelDetailInterface& DetailInterface,
		FName FullCategoryPath,
		const TArray<FString>& Categories,
		const TVoxelArray<TVoxelArray<IVoxelParameterView*>>& AllChildParameterViews,
		int32 CategoryIndex);

	void AddProviderProperty(
		const FVoxelDetailInterface& DetailInterface,
		const TSharedPtr<SBox>& ProviderDropdownContainer);
};