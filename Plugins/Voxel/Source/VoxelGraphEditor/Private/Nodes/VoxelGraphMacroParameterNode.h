// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphParameterNodeBase.h"
#include "VoxelGraphMacroParameterNode.generated.h"

UCLASS()
class UVoxelGraphMacroParameterInputNode : public UVoxelGraphParameterNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config")
	bool bExposeDefaultPin = false;

	//~ Begin UVoxelGraphParameterNodeBase Interface
	virtual EVoxelGraphParameterType GetParameterType() const override
	{
		return EVoxelGraphParameterType::Input;
	}

	virtual void AllocateParameterPins(const FVoxelGraphParameter& Parameter) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UVoxelGraphParameterNodeBase Interface
};

UCLASS()
class UVoxelGraphMacroParameterOutputNode : public UVoxelGraphParameterNodeBase
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphParameterNodeBase Interface
	virtual EVoxelGraphParameterType GetParameterType() const override
	{
		return EVoxelGraphParameterType::Output;
	}

	virtual void AllocateParameterPins(const FVoxelGraphParameter& Parameter) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UVoxelGraphParameterNodeBase Interface
};

UCLASS(Deprecated)
class UDEPRECATED_VoxelGraphMacroParameterNode : public UVoxelGraphParameterNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	EVoxelGraphParameterType Type;

	//~ Begin UVoxelGraphParameterNodeBase Interface
	virtual EVoxelGraphParameterType GetParameterType() const override
	{
		return Type;
	}
	//~ End UVoxelGraphParameterNodeBase Interface
};