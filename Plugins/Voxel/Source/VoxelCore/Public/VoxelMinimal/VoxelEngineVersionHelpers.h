// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

#define VOXEL_ENGINE_VERSION (ENGINE_MAJOR_VERSION * 100 + ENGINE_MINOR_VERSION)

#if VOXEL_ENGINE_VERSION >= 503
#define UE_503_SWITCH(Before, AfterOrEqual) AfterOrEqual
#define UE_503_ONLY(...) __VA_ARGS__
#else
#define UE_503_SWITCH(Before, AfterOrEqual) Before
#define UE_503_ONLY(...)
#endif