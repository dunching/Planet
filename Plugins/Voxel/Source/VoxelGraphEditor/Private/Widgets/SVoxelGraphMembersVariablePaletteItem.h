// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "SGraphPalette.h"

class SVoxelMembers;
struct FVoxelGraphMembersVariableSchemaAction;

class SVoxelGraphMembersVariablePaletteItem : public SGraphPaletteItem
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(TSharedPtr<SVoxelMembers>, MembersWidget)
	};

	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData);

protected:
	//~ Begin SGraphPaletteItem Interface
	virtual TSharedRef<SWidget> CreateTextSlotWidget(FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bIsReadOnly) override;
	virtual void OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit) override;
	virtual FText GetDisplayText() const override;
	//~ End SGraphPaletteItem Interface

private:
	TSharedPtr<FVoxelGraphMembersVariableSchemaAction> GetAction() const;

private:
	TWeakPtr<SVoxelMembers> WeakMembersWidget;
	int32 CategoriesCount = 0;
};