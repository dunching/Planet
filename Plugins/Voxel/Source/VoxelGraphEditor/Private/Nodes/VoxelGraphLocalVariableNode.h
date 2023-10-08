// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphParameterNodeBase.h"
#include "VoxelGraphLocalVariableNode.generated.h"

class UVoxelGraphLocalVariableUsageNode;

UCLASS(Abstract)
class UVoxelGraphLocalVariableNode : public UVoxelGraphParameterNodeBase
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphNode Interface
	virtual EVoxelGraphParameterType GetParameterType() const override
	{
		return EVoxelGraphParameterType::LocalVariable;
	}
	//~ End UVoxelGraphNode Interface
};

UCLASS()
class UVoxelGraphLocalVariableDeclarationNode : public UVoxelGraphLocalVariableNode
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphLocalVariableNode Interface
	virtual void AllocateParameterPins(const FVoxelGraphParameter& Parameter) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanJumpToDefinition() const override { return true; }
	//~ End UVoxelGraphLocalVariableNode Interface
};

UCLASS()
class UVoxelGraphLocalVariableUsageNode : public UVoxelGraphLocalVariableNode
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphLocalVariableNode Interface
	virtual void AllocateParameterPins(const FVoxelGraphParameter& Parameter) override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool CanJumpToDefinition() const override { return true; }
	virtual void JumpToDefinition() const override;
	//~ End UVoxelGraphLocalVariableNode Interface

	UVoxelGraphLocalVariableDeclarationNode* FindDeclaration() const;
};