// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

#if WITH_EDITOR
class FVoxelSourceParser;

extern VOXELGRAPHCORE_API FVoxelSourceParser* GVoxelSourceParser;

class VOXELGRAPHCORE_API FVoxelSourceParser : public FVoxelSingleton
{
public:
	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override;
	//~ End FVoxelSingleton Interface

	FString GetPinTooltip(UScriptStruct* NodeStruct, FName PinName);
	FString GetPropertyDefault(UFunction* Function, FName PropertyName);

private:
	TMap<UScriptStruct*, TMap<FName, FString>> NodeToPinToTooltip;
	TMap<UFunction*, TMap<FName, FString>> FunctionToPropertyToDefault;

	void BuildPinTooltip(UScriptStruct* NodeStruct);
};
#endif