// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"

#if WITH_EDITOR
enum class EVoxelShaderHookLocation
{
	MaterialVertexParameters,
	MaterialPixelParameters,
};

extern VOXELCORE_API void RegisterVoxelShaderHook(EVoxelShaderHookLocation Location, const FString& Text);

#define DEFINE_VOXEL_SHADER_HOOK(UniqueId, Location, Text) \
	INTELLISENSE_ONLY(void UniqueId();) \
	static const int32 PREPROCESSOR_JOIN(PREPROCESSOR_JOIN(RegisterVoxelShaderHook, UniqueId), Location) = \
		(RegisterVoxelShaderHook(EVoxelShaderHookLocation::Location, Text), 0);
#else
#define DEFINE_VOXEL_SHADER_HOOK(UniqueId, Location, Text)
#endif