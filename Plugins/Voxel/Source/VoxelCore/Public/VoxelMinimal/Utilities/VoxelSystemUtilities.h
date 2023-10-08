// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreMinimal.h"

class IPlugin;

struct VOXELCORE_API FVoxelSystemUtilities
{
	// Delay until next fire; 0 means "next frame"
	static void DelayedCall(TFunction<void()> Call, float Delay = 0);

	static IPlugin& GetPlugin();
	static FString GetAppDataCache();
	static void CleanupFileCache(const FString& Path, int64 MaxSize);
	static FString Unzip(const TArray<uint8>& Data, TMap<FString, TArray<uint8>>& OutFiles);

#if WITH_EDITOR
	static void EnsureViewportIsUpToDate();
#endif

public:
	static FString SaveFileDialog(
		const FString& DialogTitle,
		const FString& DefaultPath,
		const FString& DefaultFile,
		const FString& FileTypes);

	static bool OpenFileDialog(
		const FString& DialogTitle,
		const FString& DefaultPath,
		const FString& DefaultFile,
		const FString& FileTypes,
		bool bAllowMultiple,
		TArray<FString>& OutFilenames);

private:
	static bool FileDialog(
		bool bSave,
		const FString& DialogTitle,
		const FString& DefaultPath,
		const FString& DefaultFile,
		const FString& FileTypes,
		bool bAllowMultiple,
		TArray<FString>& OutFilenames);
};