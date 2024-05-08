// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphParameterNodeBase.h"
#include "VoxelGraphParameterNode.generated.h"

UCLASS()
class UVoxelGraphParameterNode : public UVoxelGraphParameterNodeBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool bIsBuffer = false;

public:
	//~ Begin UVoxelGraphNode Interface
	virtual EVoxelGraphParameterType GetParameterType() const override
	{
		return EVoxelGraphParameterType::Parameter;
	}

	virtual void AllocateParameterPins(const FVoxelGraphParameter& Parameter) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool CanPromotePin(const UEdGraphPin& Pin, FVoxelPinTypeSet& OutTypes) const override;
	virtual void PromotePin(UEdGraphPin& Pin, const FVoxelPinType& NewType) override;
	//~ End UVoxelGraphNode Interface
};