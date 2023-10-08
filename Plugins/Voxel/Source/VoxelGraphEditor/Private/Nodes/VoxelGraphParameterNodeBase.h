// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "VoxelGraphNode.h"
#include "VoxelGraphParameterNodeBase.generated.h"

UCLASS()
class UVoxelGraphParameterNodeBase : public UVoxelGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGuid Guid;

	UPROPERTY()
	FVoxelGraphParameter CachedParameter;

	virtual EVoxelGraphParameterType GetParameterType() const VOXEL_PURE_VIRTUAL({});

	FVoxelGraphParameter* GetParameter() const;
	const FVoxelGraphParameter& GetParameterSafe() const;

	//~ Begin UVoxelGraphNode Interface
	virtual void AllocateDefaultPins() final override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	//~ End UVoxelGraphNode Interface

protected:
	virtual void AllocateParameterPins(const FVoxelGraphParameter& Parameter) VOXEL_PURE_VIRTUAL();
};