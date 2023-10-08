// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "SchemaActions/VoxelMembersBaseSchemaAction.h"
#include "VoxelGraphMembersGraphSchemaAction.generated.h"

class UVoxelGraph;

USTRUCT()
struct FVoxelGraphMembersGraphSchemaAction : public FVoxelMembersBaseSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelMembersBaseSchemaAction::FVoxelMembersBaseSchemaAction;

	//~ Begin FVoxelGraphMembersBaseSchemaAction Interface
	virtual TSharedRef<SWidget> CreatePaletteWidget(FCreateWidgetForActionData* const InCreateData) const override;
	virtual void OnActionDoubleClick() const override;
	virtual void OnSelected() const override;
	virtual bool CanRequestRename() const override;
	virtual FString GetName() const override;
	//~ End FVoxelGraphMembersBaseSchemaAction Interface

	//~ Begin FEdGraphSchemaAction Interface
	virtual FEdGraphSchemaActionDefiningObject GetPersistentItemDefiningObject() const override;
	//~ End FEdGraphSchemaAction Interface

private:
	UEdGraph* GetEdGraph() const;

public:
	TWeakObjectPtr<UVoxelGraph> WeakGraph;
};