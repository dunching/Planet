// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphNode.h"
#include "VoxelNodeDefinition.h"
#include "VoxelFunctionCallNode.h"
#include "VoxelGraphMacroNode.generated.h"

class UVoxelGraph;
class UVoxelGraphInterface;

UCLASS()
class UVoxelGraphMacroNode : public UVoxelGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Config", DisplayName = "Graph")
	TObjectPtr<UVoxelGraphInterface> GraphInterface;

	UPROPERTY(EditAnywhere, Category = "Config")
	EVoxelGraphMacroType Type = EVoxelGraphMacroType::Macro;

	TSet<UEdGraphPin*> CachedOptionalPins;

	//~ Begin UVoxelGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool ShowPaletteIconOnNode() const override { return true; }

	virtual bool IsPinOptional(const UEdGraphPin& Pin) const override;

	virtual bool CanJumpToDefinition() const override { return true; }
	virtual void JumpToDefinition() const override;

	virtual FName GetPinCategory(const UEdGraphPin& Pin) const override;

	virtual TSharedRef<IVoxelNodeDefinition> GetNodeDefinition() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UVoxelGraphNode Interface

private:
	TSharedPtr<int32> DelegateRef;
};

class FVoxelGraphMacroNodeDefinition : public IVoxelNodeDefinition
{
public:
	explicit FVoxelGraphMacroNodeDefinition(UVoxelGraphMacroNode& Node);

	virtual TSharedPtr<const FNode> GetInputs() const override;
	virtual TSharedPtr<const FNode> GetOutputs() const override;
	TSharedPtr<const FNode> GetPins(const bool bInput) const;
	virtual bool OverridePinsOrder() const override
	{
		return true;
	}

private:
	UVoxelGraphMacroNode& Node;
};