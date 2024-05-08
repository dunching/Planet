// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "SVoxelEditCondition.h"

class FVoxelOverridableSettingsCustomization;

class FVoxelOverridableSettingsCategoryBuilder : public IDetailCustomNodeBuilder
{
public:
	FName CategoryName;
	TArray<TSharedPtr<IPropertyHandle>> Handles;
	bool bIsOverridableStruct;
	TWeakPtr<FVoxelOverridableSettingsCustomization> WeakCustomization;

	//~ Begin IDetailCustomNodeBuilder Interface
	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;
	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;
	virtual FName GetName() const override { return CategoryName; }
	//~ End IDetailCustomNodeBuilder Interface
};

class FVoxelOverridableSettingsCustomization : public IPropertyTypeCustomization
{
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	void SetupHandleEditCondition(IDetailPropertyRow& PropertyRow, const TSharedPtr<IPropertyHandle>& Handle) const;

private:
	TSharedPtr<IPropertyHandle> OverridableParametersHandle;
};

class FVoxelOverridableSettingsIdentifier : public IPropertyTypeIdentifier
{
	virtual bool IsPropertyTypeCustomized(const IPropertyHandle& PropertyHandle) const override
	{
		return !PropertyHandle.HasMetaData("NoOverrideCustomization");
	}
};