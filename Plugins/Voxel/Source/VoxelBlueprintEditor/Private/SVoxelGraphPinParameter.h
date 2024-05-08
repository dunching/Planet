// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"

class SVoxelGraphParameterComboBox;
struct FVoxelGraphBlueprintParameter;

class SVoxelGraphPinParameter : public SGraphPin
{
public:
	VOXEL_SLATE_ARGS()
	{
	};

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	//~ Begin SGraphPin Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	//~ End SGraphPin Interface

private:
	void UpdateParameterProvider(const TWeakInterfacePtr<IVoxelParameterProvider>& NewParameterProvider);
	FVoxelGraphBlueprintParameter GetCachedParameter() const;

private:
	TWeakInterfacePtr<IVoxelParameterProvider> WeakParameterProvider;
	FEdGraphPinReference AssetPinReference;
	TSharedPtr<SVoxelGraphParameterComboBox> ParameterComboBox;

	TSharedPtr<SBox> TextContainer;
	TSharedPtr<SBox> ParameterSelectorContainer;
};