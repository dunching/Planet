// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "VoxelParameter.h"
#include "K2Node_VoxelGraphParameterBase.h"

class SVoxelGraphParameterSelector;

class SVoxelGraphParameterComboBox : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnParameterChanged, FVoxelParameter)

public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(TWeakInterfacePtr<IVoxelParameterProvider>, ParameterProvider)
		SLATE_ARGUMENT(FVoxelGraphBlueprintParameter, CurrentParameter)
		SLATE_EVENT(FOnParameterChanged, OnTypeChanged)
	};

	void Construct(const FArguments& InArgs);

public:
	void UpdateParameter(const FVoxelGraphBlueprintParameter& NewParameter);
	void UpdateParameterProvider(const TWeakInterfacePtr<IVoxelParameterProvider>& NewParameterProvider);

private:
	TSharedRef<SWidget> GetMenuContent();

private:
	const FSlateBrush* GetIcon(const FVoxelPinType& PinType) const;
	FLinearColor GetColor(const FVoxelPinType& PinType) const;

private:
	TSharedPtr<SComboButton> TypeComboButton;
	TSharedPtr<SImage> MainIcon;
	TSharedPtr<STextBlock> MainTextBlock;

	TSharedPtr<SMenuOwner> MenuContent;
	TSharedPtr<SVoxelGraphParameterSelector> ParameterSelector;

private:
	TWeakInterfacePtr<IVoxelParameterProvider> CachedParameterProvider;
	bool bIsValidParameter = false;

	FOnParameterChanged OnParameterChanged;
};