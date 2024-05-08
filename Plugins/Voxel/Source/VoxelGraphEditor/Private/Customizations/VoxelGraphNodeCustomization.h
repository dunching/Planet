// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"

class IVoxelNodeDefinition;
class UVoxelGraphStructNode;

class FVoxelGraphNodeCustomization : public IDetailCustomization
{
public:
	TArray<TSharedPtr<FVoxelStructCustomizationWrapper>> Wrappers;

	FVoxelGraphNodeCustomization() = default;

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	//~ End IDetailCustomization Interface

	TSharedRef<SWidget> CreateExposePinButton(const TWeakObjectPtr<UVoxelGraphStructNode>& WeakNode, FName PinName) const;

private:
	// Keep alive
	TSharedPtr<IVoxelNodeDefinition> NodeDefinition;

	TMap<FName, TSharedPtr<IPropertyHandle>> InitializeStructChildren(const TSharedRef<IPropertyHandle>& StructHandle);
};