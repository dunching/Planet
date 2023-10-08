// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

class VOXELGRAPHCORE_API FVoxelGraphMigration : public FVoxelSingleton
{
public:
	UFunction* FindNewFunction(FName CachedName) const;
	FName FindNewPinName(UObject* Outer, FName OldName) const;

public:
	struct FNodeMigration
	{
		FName DisplayName;
		UFunction* NewFunction = nullptr;
		UScriptStruct* NewNode = nullptr;
	};
	struct FPinMigration
	{
		UObject* Outer = nullptr;
		FName OldName;
		FName NewName;
	};
	void RegisterNodeMigration(const FNodeMigration& NodeMigration);
	void RegisterPinMigration(const FPinMigration& PinMigration);

private:
	TMap<FName, FNodeMigration> NodeMigrations;
	TMap<TPair<UObject*, FName>, FPinMigration> PinMigrations;
};

extern VOXELGRAPHCORE_API FVoxelGraphMigration* GVoxelGraphMigration;

#define REGISTER_VOXEL_FUNCTION_MIGRATION(Name, Class, Function) \
	GVoxelGraphMigration->RegisterNodeMigration({ Name, FindUFunctionChecked(Class, Function) });

#define REGISTER_VOXEL_FUNCTION_PIN_MIGRATION(Class, Function, OldPin, NewPin) \
	INTELLISENSE_ONLY({ int32 OldPin, NewPin; (void)OldPin; (void)NewPin; }); \
	GVoxelGraphMigration->RegisterPinMigration({ FindUFunctionChecked(Class, Function), #OldPin, #NewPin });