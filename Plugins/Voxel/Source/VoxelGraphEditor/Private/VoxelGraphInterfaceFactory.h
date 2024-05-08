// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphInterfaceFactory.generated.h"

class UVoxelGraph;

UCLASS()
class VOXELGRAPHEDITOR_API UVoxelGraphInterfaceFactory : public UFactory
{
	GENERATED_BODY()

public:
	UVoxelGraphInterfaceFactory();

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface

public:
	UPROPERTY()
	TObjectPtr<UVoxelGraph> AssetToCopy;
};