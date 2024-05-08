// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreEditorMinimal.h"
#include "VoxelMacroLibraryFactory.generated.h"

UCLASS()
class VOXELGRAPHEDITOR_API UVoxelMacroLibraryFactory : public UFactory
{
	GENERATED_BODY()

public:
	UVoxelMacroLibraryFactory();

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	//~ End UFactory Interface
};