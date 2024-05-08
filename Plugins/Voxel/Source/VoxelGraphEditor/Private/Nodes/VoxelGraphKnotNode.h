// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphNode.h"
#include "VoxelGraphKnotNode.generated.h"

UCLASS()
class UVoxelGraphKnotNode : public UVoxelGraphNode
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual bool IsCompact() const override { return true; }

	virtual bool CanPromotePin(const UEdGraphPin& Pin, FVoxelPinTypeSet& OutTypes) const override;
	virtual void PromotePin(UEdGraphPin& Pin, const FVoxelPinType& NewType) override;

	virtual bool CanSplitPin(const UEdGraphPin& Pin) const override;
	virtual void SplitPin(UEdGraphPin& Pin) override;

	virtual void PostReconstructNode() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool ShouldOverridePinNames() const override { return true; }
	virtual FText GetPinNameOverride(const UEdGraphPin& Pin) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual UEdGraphPin* GetPassThroughPin(const UEdGraphPin* FromPin) const override;
	virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override;
	//~ End UVoxelGraphNode Interface

	void PropagatePinType();

	UEdGraphPin* GetPin(EEdGraphPinDirection Direction) const;
	TArray<UEdGraphPin*> GetLinkedPins(EEdGraphPinDirection Direction);

private:
	void PropagatePinTypeInDirection(EEdGraphPinDirection Direction);

	bool bRecursionGuard = false;
};