// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class FDetailItemNode;
class IVoxelParameterView;
class FVoxelParameterDetails;

class FVoxelParameterChildBuilder
	: public IDetailCustomNodeBuilder
	, public TSharedFromThis<FVoxelParameterChildBuilder>
{
public:
	FVoxelParameterDetails& ParameterDetails;
	const TSharedRef<SWidget> ValueWidget;

	bool bIsExpanded = false;
	TWeakPtr<const FDetailItemNode> WeakNode;
	FSimpleDelegate OnRegenerateChildren;
	TVoxelArray<TVoxelArray<IVoxelParameterView*>> ParameterViewsCommonChildren;

	FVoxelParameterChildBuilder(
		FVoxelParameterDetails& ParameterDetails,
		const TSharedRef<SWidget>& ValueWidget)
		: ParameterDetails(ParameterDetails)
		, ValueWidget(ValueWidget)
	{
	}

	void UpdateExpandedState();

	//~ Begin IDetailCustomNodeBuilder Interface
	virtual void SetOnRebuildChildren(const FSimpleDelegate InOnRegenerateChildren) override;
	virtual void GenerateHeaderRowContent(FDetailWidgetRow& Row) override;
	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;
	virtual FName GetName() const override;
	//~ End IDetailCustomNodeBuilder Interface
};