// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphNodeBase.generated.h"

class UVoxelGraphSchemaBase;
struct FVoxelGraphToolkit;
struct FVoxelGraphDelayOnGraphChangedScope;

USTRUCT()
struct FVoxelPinPreviewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Preview")
	FName PinName;

	UPROPERTY(EditAnywhere, Category = "Preview")
	FEdGraphPinType PinType;

	UPROPERTY(EditAnywhere, Category = "Preview")
	FVoxelInstancedStruct PreviewHandler;
};

UCLASS(Abstract)
class UVoxelGraphNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	bool bEnableDebug = false;

	UPROPERTY()
	bool bEnablePreview = false;

	UPROPERTY(EditAnywhere, Category = "Preview")
	FName PreviewedPin;

	UPROPERTY(EditAnywhere, Category = "Preview")
	TArray<FVoxelPinPreviewSettings> PreviewSettings;

	UPROPERTY()
	TSet<FName> CollapsedInputCategories;

	UPROPERTY()
	TSet<FName> CollapsedOutputCategories;

	UPROPERTY()
	bool bShowAdvanced = false;

	TSet<FName> InputPinCategories;
	TSet<FName> OutputPinCategories;

	TArray<UEdGraphPin*> GetInputPins() const;
	TArray<UEdGraphPin*> GetOutputPins() const;

	UEdGraphPin* GetInputPin(int32 Index) const;
	UEdGraphPin* GetOutputPin(int32 Index) const;

	UEdGraphPin* FindPinByPredicate_Unique(TFunctionRef<bool(UEdGraphPin* Pin)> Function) const;

	const UVoxelGraphSchemaBase* GetSchema() const;
	TSharedPtr<FVoxelGraphToolkit> GetToolkit() const;
	void RefreshNode();

	virtual bool IsCompact() const { return false; }
	virtual bool CanRenameOnSpawn() const { return true; }
	virtual bool ShowAsPromotableWildcard(const UEdGraphPin& Pin) const { return false; }
	virtual FName GetPinCategory(const UEdGraphPin& Pin) const { return {}; }
	virtual bool ShouldHideConnectorPin(const UEdGraphPin& Pin) const { return false; }
	virtual bool IsPinOptional(const UEdGraphPin& Pin) const { return false; }
	virtual bool ShouldHidePinDefaultValue(const UEdGraphPin& Pin) const { return false; }
	virtual bool GetOverlayInfo(FString& Type, FString& Tooltip, FString& Color) { return false; }
	virtual bool HasExecutionFlow() { return false; }

	virtual bool CanPasteVoxelNode(const TSet<UEdGraphNode*>& PastedNodes) { return true; }
	virtual void PostPasteVoxelNode(const TSet<UEdGraphNode*>& PastedNodes) {}

	virtual bool CanRemovePin_ContextMenu(const UEdGraphPin& Pin) const { return false; }
	virtual void RemovePin_ContextMenu(UEdGraphPin& Pin) VOXEL_PURE_VIRTUAL();

	virtual bool CanSplitPin(const UEdGraphPin& Pin) const { return false; }
	virtual void SplitPin(UEdGraphPin& Pin) VOXEL_PURE_VIRTUAL();

	virtual bool CanRecombinePin(const UEdGraphPin& Pin) const;
	virtual void RecombinePin(UEdGraphPin& Pin);

	virtual bool TryMigratePin(UEdGraphPin* OldPin, UEdGraphPin* NewPin) const VOXEL_PURE_VIRTUAL({});
	virtual void TryMigrateDefaultValue(const UEdGraphPin* OldPin, UEdGraphPin* NewPin) const {}

	virtual FName GetMigratedPinName(const FName PinName) const { return PinName; }

	virtual void PreReconstructNode() {}
	virtual void PostReconstructNode() {}

private:
	virtual bool CanSplitPin(const UEdGraphPin* Pin) const final override
	{
		return ensure(Pin) && CanSplitPin(*Pin);
	}

public:
	static void FocusOnNode(UEdGraphNode* Node);
	static void FocusOnNodes(const TArray<UEdGraphNode*>& Nodes);

public:
	void ReconstructNode(bool bCreateOrphans);
	void FixupPreviewSettings();

	//~ Begin UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	virtual void ReconstructNode() final override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override VOXEL_PURE_VIRTUAL(false);
	//~ End UEdGraphNode interface

	//~ Begin UObject interface
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject interface
};