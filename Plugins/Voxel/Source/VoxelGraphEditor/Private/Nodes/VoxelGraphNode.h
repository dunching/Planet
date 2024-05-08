// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelPinValue.h"
#include "VoxelGraphNodeBase.h"
#include "VoxelGraphNode.generated.h"

class IVoxelNodeDefinition;

UCLASS(Abstract)
class UVoxelGraphNode : public UVoxelGraphNodeBase
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphNode Interface
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual bool CanSplitPin(const UEdGraphPin& Pin) const override;
	virtual void SplitPin(UEdGraphPin& Pin) override;
	//~ End UVoxelGraphNode Interface

	virtual bool CanPromotePin(const UEdGraphPin& Pin, FVoxelPinTypeSet& OutTypes) const { return false; }
	virtual void PromotePin(UEdGraphPin& Pin, const FVoxelPinType& NewType) VOXEL_PURE_VIRTUAL();
	virtual FString GetPinPromotionWarning(const UEdGraphPin& Pin, const FVoxelPinType& NewType) const { return {}; }

	virtual TSharedRef<IVoxelNodeDefinition> GetNodeDefinition();

public:
	virtual bool TryMigratePin(UEdGraphPin* OldPin, UEdGraphPin* NewPin) const override;
	virtual void TryMigrateDefaultValue(const UEdGraphPin* OldPin, UEdGraphPin* NewPin) const override;
	virtual void PostReconstructNode() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
};