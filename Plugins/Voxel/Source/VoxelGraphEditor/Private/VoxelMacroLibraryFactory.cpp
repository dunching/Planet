// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMacroLibraryFactory.h"

#include "VoxelEdGraph.h"
#include "VoxelGraphSchema.h"
#include "VoxelMacroLibrary.h"

UVoxelMacroLibraryFactory::UVoxelMacroLibraryFactory()
{
	SupportedClass = UVoxelMacroLibrary::StaticClass();

	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UVoxelMacroLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, const FName Name, const EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UVoxelMacroLibrary* MacroLibrary = NewObject<UVoxelMacroLibrary>(InParent, Class, Name, Flags);
	if (!MacroLibrary)
	{
		return nullptr;
	}

	UVoxelGraph* NewMacro = NewObject<UVoxelGraph>(MacroLibrary->Graph, NAME_None, RF_Transactional | RF_Public);
	NewMacro->SetGraphName(Name.ToString());
	NewMacro->Category = "";
	NewMacro->MainEdGraph = NewObject<UVoxelEdGraph>(NewMacro, NAME_None, RF_Transactional);
	NewMacro->MainEdGraph->Schema = UVoxelGraphSchema::StaticClass();

	MacroLibrary->Graph->InlineMacros.Add(NewMacro);
	MacroLibrary->PostEditChange();

	return MacroLibrary;
}