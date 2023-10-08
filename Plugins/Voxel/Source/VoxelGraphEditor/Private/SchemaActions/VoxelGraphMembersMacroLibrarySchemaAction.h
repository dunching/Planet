// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphMembersMacroSchemaAction.h"
#include "VoxelGraphMembersMacroLibrarySchemaAction.generated.h"

USTRUCT()
struct FVoxelGraphMembersMacroLibrarySchemaAction : public FVoxelGraphMembersMacroSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelGraphMembersMacroSchemaAction::FVoxelGraphMembersMacroSchemaAction;

	//~ Begin FVoxelGraphMembersBaseSchemaAction Interface
	virtual TSharedRef<SWidget> CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const override;
	virtual void OnDelete() const override;
	//~ End FVoxelGraphMembersBaseSchemaAction Interface

	bool IsMacroVisible() const;
	void ToggleMacroVisibility() const;
};