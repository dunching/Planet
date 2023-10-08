// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VoxelBaseNode.generated.h"

struct FVoxelPinType;
struct FVoxelPinTypeSet;

UCLASS(Abstract)
class VOXELBLUEPRINT_API UK2Node_VoxelBaseNode : public UK2Node_CallFunction
{
	GENERATED_BODY()

public:
	//~ Begin UK2Node Interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//~ End UK2Node Interface

	void AddConvertPinContextAction(UToolMenu* Menu, UGraphNodeContextMenuContext* Context, const FVoxelPinTypeSet& TypeSet) const;
	bool CanAutoConvert(const UEdGraphPin& Pin, const UEdGraphPin& OtherPin, FString& OutReason) const;

	virtual bool IsPinWildcard(const UEdGraphPin& Pin) const
	{
		return false;
	}

	virtual void OnPinTypeChange(UEdGraphPin& Pin, const FVoxelPinType& NewType) VOXEL_PURE_VIRTUAL();
};