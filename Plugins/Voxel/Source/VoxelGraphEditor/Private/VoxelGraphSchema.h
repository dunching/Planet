// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "VoxelGraphSchemaBase.h"
#include "VoxelGraphSchemaAction.h"
#include "VoxelGraphSchema.generated.h"

struct FVoxelNode;
struct FVoxelGraphToolkit;

USTRUCT()
struct FVoxelGraphSchemaAction_NewMacroNode : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	UPROPERTY()
	TObjectPtr<UVoxelGraphInterface> Graph = nullptr;

	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void GetIcon(FSlateIcon& Icon, FLinearColor& Color) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewParameterUsage : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	UPROPERTY()
	FGuid Guid;

	UPROPERTY()
	EVoxelGraphParameterType ParameterType = {};

	UPROPERTY()
	FVoxelPinType PinType;

	UPROPERTY()
	bool bInput_ExposeDefaultAsPin = false;

	UPROPERTY()
	bool bLocalVariable_IsDeclaration = false;

	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void GetIcon(FSlateIcon& Icon, FLinearColor& Color) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewParameter : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	UPROPERTY()
	EVoxelGraphParameterType ParameterType = {};

	TOptional<FVoxelPinType> PinType;
	FName ParameterName;

	UPROPERTY()
	FString TargetCategory;

	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewInlineMacro : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	UPROPERTY()
	FString TargetCategory;

	UPROPERTY()
	TObjectPtr<UVoxelGraph> NewMacro;

	bool bOpenNewGraph = true;

	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewStructNode : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	const FVoxelNode* Node = nullptr;

	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void GetIcon(FSlateIcon& Icon, FLinearColor& Color) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewPromotableStructNode : public FVoxelGraphSchemaAction_NewStructNode
{
	GENERATED_BODY();

public:
	TArray<FVoxelPinType> PinTypes;

	using FVoxelGraphSchemaAction_NewStructNode::FVoxelGraphSchemaAction_NewStructNode;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

USTRUCT()
struct FVoxelGraphSchemaAction_NewKnotNode : public FVoxelGraphSchemaAction
{
	GENERATED_BODY();

public:
	using FVoxelGraphSchemaAction::FVoxelGraphSchemaAction;
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void GetIcon(FSlateIcon& Icon, FLinearColor& Color) override;
};

UCLASS()
class UVoxelGraphSchema : public UVoxelGraphSchemaBase
{
	GENERATED_BODY()

public:
	//~ Begin UVoxelGraphSchema Interface
	virtual TSharedPtr<FEdGraphSchemaAction> FindCastAction(const FEdGraphPinType& From, const FEdGraphPinType& To) const override;
	virtual TOptional<FPinConnectionResponse> GetCanCreateConnectionOverride(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const override;
	virtual bool CreatePromotedConnectionSafe(UEdGraphPin*& PinA, UEdGraphPin*& PinB) const override;

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const override;

	virtual void GetAssetsGraphHoverMessage(const TArray<FAssetData>& Assets, const UEdGraph* HoverGraph, FString& OutTooltipText, bool& OutOkIcon) const override;
	virtual void DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const override;
	//~ End UVoxelGraphSchema Interface

	static TSharedPtr<FVoxelGraphToolkit> GetToolkit(const UEdGraph* Graph);

private:
	bool TryGetPromotionType(const UEdGraphPin& Pin, const FVoxelPinType& TargetType, FVoxelPinType& OutType, FString& OutAdditionalText) const;
	void PromoteToVariable(UEdGraphPin* Pin, EVoxelGraphParameterType ParameterType) const;
	void PromoteToMakeValue(UEdGraphPin* Pin) const;
	static TMap<FVoxelPinType, TSet<FVoxelPinType>> CollectOperatorPermutations(const FVoxelNode& Node, const UEdGraphPin& FromPin, const FVoxelPinTypeSet& PromotionTypes);
};