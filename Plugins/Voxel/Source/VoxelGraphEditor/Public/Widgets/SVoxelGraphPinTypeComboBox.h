// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelPinType.h"

class SVoxelPinTypeSelector;

enum class EVoxelPinContainerType : uint8
{
	None,
	Buffer,
	BufferArray
};

class VOXELGRAPHEDITOR_API SVoxelPinTypeComboBox : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnTypeChanged, FVoxelPinType)

public:
	VOXEL_SLATE_ARGS()
	{
		FArguments()
			: _DetailsWindow(true)
		{
		}

		SLATE_ATTRIBUTE(FVoxelPinTypeSet, AllowedTypes)
		SLATE_ATTRIBUTE(FVoxelPinType, CurrentType)
		SLATE_ARGUMENT(bool, DetailsWindow)
		SLATE_ATTRIBUTE(bool, ReadOnly)
		SLATE_EVENT(FOnTypeChanged, OnTypeChanged)
	};

	void Construct(const FArguments& InArgs);

	//~ Begin SCompoundWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~ End SCompoundWidget Interface

private:
	TSharedRef<SWidget> GetMenuContent();

	TSharedRef<SWidget> GetPinContainerTypeMenuContent();
	void OnContainerTypeSelectionChanged(EVoxelPinContainerType ContainerType) const;

	void UpdateType(const FVoxelPinType& NewType);

private:
	const FSlateBrush* GetIcon(const FVoxelPinType& PinType) const;
	FLinearColor GetColor(const FVoxelPinType& PinType) const;

private:
	TSharedPtr<SComboButton> TypeComboButton;
	TSharedPtr<SComboButton> ContainerTypeComboButton;
	TSharedPtr<SImage> MainIcon;
	TSharedPtr<STextBlock> MainTextBlock;

	TSharedPtr<SMenuOwner> MenuContent;
	TSharedPtr<SMenuOwner> ContainerTypeMenuContent;
	TSharedPtr<SVoxelPinTypeSelector> PinTypeSelector;

private:
	TAttribute<FVoxelPinTypeSet> AllowedTypes;
	TAttribute<FVoxelPinType> CurrentType;

	bool bAllowUniforms = false;
	bool bAllowBuffers = false;
	bool bAllowBufferArrays = false;

	FVoxelPinType CachedType;
	FVoxelPinType CachedInnerType;

	TAttribute<bool> ReadOnly;
	FOnTypeChanged OnTypeChanged;
};