// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "K2Node_VoxelGraphParameterBase.h"
#include "K2Node_SetVoxelGraphParameter.generated.h"

class UVoxelGraph;

UCLASS()
class VOXELBLUEPRINT_API UK2Node_SetVoxelGraphParameter : public UK2Node_VoxelGraphParameterBase
{
	GENERATED_BODY()

public:
	UK2Node_SetVoxelGraphParameter();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//~ End UK2Node Interface

	//~ Begin UK2Node_VoxelGraphParameterBase Interface
	virtual bool IsPinWildcard(const UEdGraphPin& Pin) const override;
	virtual UEdGraphPin* GetParameterNamePin() const override;
	//~ End UK2Node_VoxelGraphParameterBase Interface
};