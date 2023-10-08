// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreEditorMinimal.h"

class VOXELCOREEDITOR_API FVoxelStructCustomizationWrapper : public TSharedFromThis<FVoxelStructCustomizationWrapper>
{
public:
	static TSharedPtr<FVoxelStructCustomizationWrapper> Make(const TSharedRef<IPropertyHandle>& StructHandle);

	VOXEL_COUNT_INSTANCES();

	TArray<TSharedPtr<IPropertyHandle>> AddChildStructure();

	IDetailPropertyRow* AddExternalStructure(
		const FVoxelDetailInterface& DetailInterface,
		const FAddPropertyParams& Params = FAddPropertyParams());

private:
	const TSharedRef<IPropertyHandle> StructHandle;
	const TSharedRef<FStructOnScope> StructOnScope;

	double LastSyncTime = 0.0;
	uint64 LastPostChangeFrame = -1;

	FVoxelStructCustomizationWrapper(
		const TSharedRef<IPropertyHandle>& StructHandle,
		const TSharedRef<FStructOnScope>& StructOnScope)
		: StructHandle(StructHandle)
		, StructOnScope(StructOnScope)
	{
	}

	void SyncFromSource() const;
	void SyncToSource() const;
	void SetupChildHandle(const TSharedPtr<IPropertyHandle>& Handle);

	friend class FVoxelStructCustomizationWrapperTicker;
};