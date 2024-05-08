// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinType.h"
#include "K2Node_VoxelBaseNode.h"
#include "K2Node_VoxelPointAttributes.generated.h"

UCLASS(Abstract)
class VOXELBLUEPRINT_API UK2Node_VoxelPointAttributeBase : public UK2Node_VoxelBaseNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void PostReconstructNode() override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	//~ End UK2Node Interface

	//~ Begin UK2Node_VoxelBaseNode Interface
	virtual void OnPinTypeChange(UEdGraphPin& Pin, const FVoxelPinType& NewType) override;
	virtual bool IsPinWildcard(const UEdGraphPin& Pin) const override;
	//~ End UK2Node_VoxelBaseNode Interface

private:
	void SetType(UEdGraphPin& Pin, const FVoxelPinType& NewType);

private:
	UPROPERTY()
	FVoxelPinType ValueType;
};

UCLASS()
class VOXELBLUEPRINT_API UK2Node_VoxelSetPointAttribute : public UK2Node_VoxelPointAttributeBase
{
	GENERATED_BODY()

	UK2Node_VoxelSetPointAttribute();
};

UCLASS()
class VOXELBLUEPRINT_API UK2Node_VoxelGetPointAttribute : public UK2Node_VoxelPointAttributeBase
{
	GENERATED_BODY()

	UK2Node_VoxelGetPointAttribute();
};