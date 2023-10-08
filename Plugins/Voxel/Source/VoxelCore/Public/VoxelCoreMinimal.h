// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

// ReSharper disable CppUnusedIncludeDirective

#include "VoxelMinimal/VoxelEngineVersionHelpers.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "EngineUtils.h"
#include "PixelFormat.h"
#include "MaterialDomain.h"
#include "RenderingThread.h"
#include "DrawDebugHelpers.h"
#include "Async/Async.h"
#include "Containers/Queue.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Misc/EnumRange.h"
#include "Misc/FileHelper.h"
#include "UObject/Package.h"
#include "UObject/GCObject.h"
#include "UObject/ObjectKey.h"
#include "UObject/UObjectIterator.h"
#include "UObject/WeakInterfacePtr.h"
#include "Engine/World.h"
#include "Engine/Texture.h"
#include "Engine/EngineBaseTypes.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class UEditorEngine;
class FPrimitiveSceneProxy;

#if WITH_EDITOR
extern UNREALED_API UEditorEngine* GEditor;
#endif

// Fix clang errors due to long long vs long issues
#define int64_t int64
#define uint64_t uint64

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "VoxelMinimal/VoxelLog.h"
#include "VoxelMinimal/VoxelStats.h"
#include "VoxelMinimal/VoxelMacros.h"
#include "VoxelMinimal/VoxelSharedPtr.h"
#include "VoxelMinimal/VoxelEngineHelpers.h"
#include "VoxelMinimal/VoxelDelegateHelpers.h"
#include "VoxelMinimal/VoxelMemory.h"