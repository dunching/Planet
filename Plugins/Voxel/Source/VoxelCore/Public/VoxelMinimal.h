// Copyright Voxel Plugin, Inc. All Rights Reserved.

#ifdef CANNOT_INCLUDE_VOXEL_MINIMAL
#error "VoxelMinimal.h recursively included"
#endif

#ifndef VOXEL_MINIMAL_INCLUDED
#define VOXEL_MINIMAL_INCLUDED
#define CANNOT_INCLUDE_VOXEL_MINIMAL

// Useful when compiling with clang
// Add this to "%appdata%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml"
/*
<?xml version="1.0" encoding="utf-8" ?>
<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration">
    <WindowsPlatform>
         <Compiler>Clang</Compiler>
    </WindowsPlatform>
	<ParallelExecutor>
		<ProcessorCountMultiplier>2</ProcessorCountMultiplier>
		<MemoryPerActionBytes>0</MemoryPerActionBytes>
	</ParallelExecutor>
</Configuration>
*/
#if 0
#undef DLLEXPORT
#undef DLLIMPORT
#define DLLEXPORT
#define DLLIMPORT
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "VoxelCoreMinimal.h"

#include "VoxelMinimal/VoxelCriticalSection.h"
#include "VoxelMinimal/VoxelUniqueFunction.h"
#include "VoxelMinimal/VoxelImpl.h"
#include "VoxelMinimal/VoxelBox.h"
#include "VoxelMinimal/VoxelBox2D.h"
#include "VoxelMinimal/VoxelIntBox.h"
#include "VoxelMinimal/VoxelGuid.h"
#include "VoxelMinimal/VoxelHash.h"
#include "VoxelMinimal/VoxelAxis.h"
#include "VoxelMinimal/VoxelMessages.h"
#include "VoxelMinimal/VoxelTextureRef.h"
#include "VoxelMinimal/VoxelMaterialRef.h"
#include "VoxelMinimal/VoxelTicker.h"
#include "VoxelMinimal/VoxelAutoFactoryInterface.h"
#include "VoxelMinimal/VoxelInstancedStruct.h"
#include "VoxelMinimal/VoxelRange.h"
#include "VoxelMinimal/VoxelISPC.h"
#include "VoxelMinimal/VoxelSuccess.h"
#include "VoxelMinimal/VoxelSingleton.h"
#include "VoxelMinimal/VoxelStructView.h"
#include "VoxelMinimal/VoxelVirtualStruct.h"
#include "VoxelMinimal/VoxelWorldSubsystem.h"
#include "VoxelMinimal/VoxelOverridableSettings.h"

#include "VoxelMinimal/Containers/VoxelMap.h"
#include "VoxelMinimal/Containers/VoxelArray.h"
#include "VoxelMinimal/Containers/VoxelArrayView.h"
#include "VoxelMinimal/Containers/VoxelBitArray.h"
#include "VoxelMinimal/Containers/VoxelSparseArray.h"
#include "VoxelMinimal/Containers/VoxelChunkedArray.h"
#include "VoxelMinimal/Containers/VoxelChunkedSparseArray.h"
#include "VoxelMinimal/Containers/VoxelAddOnlyMap.h"
#include "VoxelMinimal/Containers/VoxelAddOnlySet.h"
#include "VoxelMinimal/Containers/VoxelPackedData.h"
#include "VoxelMinimal/Containers/VoxelPackedArray.h"
#include "VoxelMinimal/Containers/VoxelPackedArrayView.h"
#include "VoxelMinimal/Containers/VoxelPaletteArray.h"

#include "VoxelMinimal/Utilities/VoxelGameUtilities.h"
#include "VoxelMinimal/Utilities/VoxelMathUtilities.h"
#include "VoxelMinimal/Utilities/VoxelSystemUtilities.h"
#include "VoxelMinimal/Utilities/VoxelRenderUtilities.h"
#include "VoxelMinimal/Utilities/VoxelObjectUtilities.h"
#include "VoxelMinimal/Utilities/VoxelTextureUtilities.h"
#include "VoxelMinimal/Utilities/VoxelThreadingUtilities.h"
#include "VoxelMinimal/Utilities/VoxelDistanceFieldUtilities.h"

#undef CANNOT_INCLUDE_VOXEL_MINIMAL
#endif