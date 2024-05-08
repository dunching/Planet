// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelParameter.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VoxelBaseNode.h"
#include "K2Node_VoxelGraphParameterBase.generated.h"

class UVoxelGraph;
class IVoxelParameterProvider;

USTRUCT()
struct FVoxelGraphBlueprintParameter
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid Guid;

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FVoxelPinType Type;

	UPROPERTY()
	bool bIsValid = true;

	FVoxelGraphBlueprintParameter() = default;
	explicit FVoxelGraphBlueprintParameter(const FVoxelParameter& Parameter)
		: Guid(Parameter.Guid)
		, Name(Parameter.Name)
		, Type(Parameter.Type.GetExposedType())
	{
	}

	FString GetValue() const
	{
		return Guid.IsValid() ? Guid.ToString() : Name.ToString();
	}
};

UCLASS(Abstract)
class VOXELBLUEPRINT_API UK2Node_VoxelGraphParameterBase : public UK2Node_VoxelBaseNode
{
	GENERATED_BODY()

public:
	UK2Node_VoxelGraphParameterBase();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PostReconstructNode() override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void PostLoad() override;
	//~ End UK2Node Interface

	virtual UEdGraphPin* GetParameterNamePin() const
	{
		return nullptr;
	}

	//~ Begin UK2Node_VoxelBaseNode Interface
	virtual void OnPinTypeChange(UEdGraphPin& Pin, const FVoxelPinType& NewType) override;
	//~ End UK2Node_VoxelBaseNode Interface

private:
	void FixupParameter();
	void SetParameter(FVoxelGraphBlueprintParameter NewParameter);

public:
	const static FName AssetPinName;
	const static FName ParameterPinName;

	UPROPERTY()
	FVoxelGraphBlueprintParameter CachedParameter;

	UPROPERTY()
	TScriptInterface<IVoxelParameterProvider> CachedParameterProvider;

private:
	FSharedVoidPtr OnGraphChangedHandleOwnerPtr;
};