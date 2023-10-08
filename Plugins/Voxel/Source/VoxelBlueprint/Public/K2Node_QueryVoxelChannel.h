// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinType.h"
#include "K2Node_VoxelBaseNode.h"
#include "K2Node_QueryVoxelChannel.generated.h"

UCLASS(Abstract)
class VOXELBLUEPRINT_API UK2Node_QueryVoxelChannelBase : public UK2Node_VoxelBaseNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode Interface
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PostReconstructNode() override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void PostLoad() override;
	//~ End UK2Node Interface

	//~ Begin UK2Node_VoxelBaseNode Interface
	virtual bool IsPinWildcard(const UEdGraphPin& Pin) const override;
	virtual void OnPinTypeChange(UEdGraphPin& Pin, const FVoxelPinType& NewType) override;
	//~ End UK2Node_VoxelBaseNode Interface

protected:
	virtual FEdGraphPinType GetValuePinType() const
	{
		return ValueType.GetEdGraphPinType_K2();
	}

protected:
	void SetType(const FVoxelPinType& NewType);
	bool HasChannel() const;
	void UpdateChannel();

protected:
	UPROPERTY()
	FVoxelPinType ValueType;

	TSharedPtr<int32> DelegateOwner;
};

UCLASS()
class VOXELBLUEPRINT_API UK2Node_QueryVoxelChannel : public UK2Node_QueryVoxelChannelBase
{
	GENERATED_BODY()

public:
	UK2Node_QueryVoxelChannel();
};

UCLASS()
class VOXELBLUEPRINT_API UK2Node_MultiQueryVoxelChannel : public UK2Node_QueryVoxelChannelBase
{
	GENERATED_BODY()

public:
	UK2Node_MultiQueryVoxelChannel();

	virtual FEdGraphPinType GetValuePinType() const override;
};