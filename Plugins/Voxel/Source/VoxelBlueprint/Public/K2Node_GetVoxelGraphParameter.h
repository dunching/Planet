// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "K2Node_VoxelGraphParameterBase.h"
#include "K2Node_GetVoxelGraphParameter.generated.h"

class UVoxelGraph;

UCLASS()
class VOXELBLUEPRINT_API UK2Node_GetVoxelGraphParameter : public UK2Node_VoxelGraphParameterBase
{
	GENERATED_BODY()

public:
	UK2Node_GetVoxelGraphParameter();

	//~ Begin UK2Node_VoxelGraphParameterBase Interface
	virtual bool IsPinWildcard(const UEdGraphPin& Pin) const override;
	virtual UEdGraphPin* GetParameterNamePin() const override;
	//~ End UK2Node_VoxelGraphParameterBase Interface
};