// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

struct FVoxelPinType;

class VOXELGRAPHEDITOR_API FVoxelPinValueCustomizationHelper
{
public:
	static TSharedPtr<FVoxelStructCustomizationWrapper> CreatePinValueCustomization(
		const TSharedRef<IPropertyHandle>& PropertyHandle,
		const FVoxelDetailInterface& DetailInterface,
		const TMap<FName, FString>& MetaData,
		const TFunctionRef<void(FDetailWidgetRow&, const TSharedRef<SWidget>&)> SetupRow,
		const FAddPropertyParams& Params,
		const TAttribute<bool>& IsEnabled = true);

	static void CreatePinValueRangeSetter(FDetailWidgetRow& Row,
		const TSharedRef<IPropertyHandle>& PropertyHandle,
		const FText& Name,
		const FText& ToolTip,
		FName Min,
		FName Max,
		const TFunction<TMap<FName, FString>(const TSharedPtr<IPropertyHandle>&)>& GetMetaData,
		const TFunction<void(const TSharedPtr<IPropertyHandle>&, const TMap<FName, FString>&)>& SetMetaData);

	static float GetValueWidgetWidthByType(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FVoxelPinType& Type);

private:
	static TSharedPtr<SWidget> AddArrayItemOptions(const TSharedRef<IPropertyHandle>& PropertyHandle, const TSharedPtr<SWidget>& ValueWidget);
};