// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.h"
#include "Interfaces/IPluginManager.h"
#include "Application/ThrottleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "miniz.h"

#if WITH_EDITOR
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "EditorViewportClient.h"
#endif

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/COMPointer.h"
#include <commdlg.h>
#include <shlobj.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

void FVoxelSystemUtilities::DelayedCall(TFunction<void()> Call, float Delay)
{
	// Delay will be inaccurate if not on game thread but that's fine
	FVoxelUtilities::RunOnGameThread([=]
	{
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float)
		{
			VOXEL_FUNCTION_COUNTER();
			Call();
			return false;
		}), Delay);
	});
}

IPlugin& FVoxelSystemUtilities::GetPlugin()
{
	static TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("Voxel");
	if (!Plugin)
	{
		Plugin = IPluginManager::Get().FindPlugin("Voxel-dev");
	}
	if (!Plugin)
	{
		for (const TSharedRef<IPlugin>& OtherPlugin : IPluginManager::Get().GetEnabledPlugins())
		{
			if (OtherPlugin->GetName().StartsWith("Voxel-2"))
			{
				ensure(!Plugin);
				Plugin = OtherPlugin;
			}
		}
	}
	return *Plugin;
}

FString FVoxelSystemUtilities::GetAppDataCache()
{
	static FString Path = FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA")) / "UnrealEngine" / "VoxelPlugin";
	return Path;
}

void FVoxelSystemUtilities::CleanupFileCache(const FString& Path, const int64 MaxSize)
{
	VOXEL_FUNCTION_COUNTER();

	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *Path, TEXT("*"), true, false);

	int64 TotalSize = 0;
	for (const FString& File : Files)
	{
		TotalSize += IFileManager::Get().FileSize(*File);
	}

	while (
		TotalSize > MaxSize &&
		ensure(Files.Num() > 0))
	{
		FString OldestFile;
		FDateTime OldestFileTimestamp;
		for (const FString& File : Files)
		{
			const FDateTime Timestamp = IFileManager::Get().GetTimeStamp(*File);

			if (OldestFile.IsEmpty() ||
				Timestamp < OldestFileTimestamp)
			{
				OldestFile = File;
				OldestFileTimestamp = Timestamp;
			}
		}
		LOG_VOXEL(Log, "Deleting %s", *OldestFile);

		TotalSize -= IFileManager::Get().FileSize(*OldestFile);
		ensure(IFileManager::Get().Delete(*OldestFile));
		ensure(Files.Remove(OldestFile));
	}
}

FString FVoxelSystemUtilities::Unzip(const TArray<uint8>& Data, TMap<FString, TArray<uint8>>& OutFiles)
{
	VOXEL_FUNCTION_COUNTER();

#define CheckZip(...) \
		if ((__VA_ARGS__) != MZ_TRUE) \
		{ \
			return FString(mz_zip_get_error_string(mz_zip_peek_last_error(&Zip))); \
		} \
		{ \
			const mz_zip_error Error = mz_zip_peek_last_error(&Zip); \
			if (Error != MZ_ZIP_NO_ERROR) \
			{ \
				return FString(mz_zip_get_error_string(Error)); \
			} \
		}

#define CheckZipError() CheckZip(MZ_TRUE)

	mz_zip_archive Zip;
	mz_zip_zero_struct(&Zip);
	ON_SCOPE_EXIT
	{
		mz_zip_end(&Zip);
	};

	CheckZip(mz_zip_reader_init_mem(&Zip, Data.GetData(), Data.Num(), 0));

	const int32 NumFiles = mz_zip_reader_get_num_files(&Zip);

	for (int32 FileIndex = 0; FileIndex < NumFiles; FileIndex++)
	{
		const int32 FilenameSize = mz_zip_reader_get_filename(&Zip, FileIndex, nullptr, 0);
		CheckZipError();

		TArray<char> FilenameBuffer;
		FilenameBuffer.SetNumUninitialized(FilenameSize);
		mz_zip_reader_get_filename(&Zip, FileIndex, FilenameBuffer.GetData(), FilenameBuffer.Num());
		CheckZipError();

		// To be extra safe
		FilenameBuffer.Add(0);

		const FString Filename = FString(FilenameBuffer.GetData());
		if (Filename.EndsWith("/"))
		{
			continue;
		}

		mz_zip_archive_file_stat FileStat;
		CheckZip(mz_zip_reader_file_stat(&Zip, FileIndex, &FileStat));

		TArray<uint8> Buffer;
		Buffer.SetNumUninitialized(FileStat.m_uncomp_size);

		CheckZip(mz_zip_reader_extract_file_to_mem(&Zip, FilenameBuffer.GetData(), Buffer.GetData(), Buffer.Num(), 0));

		ensure(!OutFiles.Contains(Filename));
		OutFiles.Add(Filename, MoveTemp(Buffer));
	}

	return {};

#undef CheckZipError
#undef CheckZip
}

#if WITH_EDITOR
void FVoxelSystemUtilities::EnsureViewportIsUpToDate()
{
	VOXEL_FUNCTION_COUNTER();

	if (FSlateThrottleManager::Get().IsAllowingExpensiveTasks())
	{
		// No need to do anything, slate is not throttling
		return;
	}

	const FViewport* Viewport = GEditor->GetActiveViewport();
	if (!Viewport)
	{
		return;
	}

	const FViewportClient* Client = Viewport->GetClient();
	if (!Client)
	{
		return;
	}

	for (FEditorViewportClient* EditorViewportClient : GEditor->GetAllViewportClients())
	{
		EditorViewportClient->Invalidate(false, false);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelSystemUtilities::SaveFileDialog(
	const FString& DialogTitle,
	const FString& DefaultPath,
	const FString& DefaultFile,
	const FString& FileTypes)
{
	TArray<FString> Filenames;
	if (!FileDialog(true, DialogTitle, DefaultPath, DefaultFile, FileTypes, false, Filenames))
	{
		return {};
	}

	if (!ensure(Filenames.Num() == 1))
	{
		return {};
	}

	return Filenames[0];
}

bool FVoxelSystemUtilities::OpenFileDialog(
	const FString& DialogTitle,
	const FString& DefaultPath,
	const FString& DefaultFile,
	const FString& FileTypes,
	bool bAllowMultiple,
	TArray<FString>& OutFilenames)
{
	return FileDialog(false, DialogTitle, DefaultPath, DefaultFile, FileTypes, bAllowMultiple, OutFilenames);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelSystemUtilities::FileDialog(
	bool bSave,
	const FString& DialogTitle,
	const FString& DefaultPath,
	const FString& DefaultFile,
	const FString& FileTypes,
	bool bAllowMultiple,
	TArray<FString>& OutFilenames)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(!bSave || !bAllowMultiple);

#if WITH_EDITOR
	{
		const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		if (DesktopPlatform)
		{
			if (bSave)
			{
				return DesktopPlatform->SaveFileDialog(
					ParentWindowHandle,
					DialogTitle,
					DefaultPath,
					DefaultFile,
					FileTypes,
					EFileDialogFlags::None,
					OutFilenames);
			}
			else
			{
				return DesktopPlatform->OpenFileDialog(
					ParentWindowHandle,
					DialogTitle,
					DefaultPath,
					DefaultFile,
					FileTypes,
					bAllowMultiple ? EFileDialogFlags::Multiple : EFileDialogFlags::None,
					OutFilenames);
			}
		}
	}
#endif

#if PLATFORM_WINDOWS
	TComPtr<IFileDialog> FileDialog;
	if (!SUCCEEDED(CoCreateInstance(
		bSave ? CLSID_FileSaveDialog : CLSID_FileOpenDialog,
		nullptr,
		CLSCTX_INPROC_SERVER,
		bSave ? IID_IFileSaveDialog : IID_IFileOpenDialog,
		IID_PPV_ARGS_Helper(&FileDialog))))
	{
		return false;
	}

	if (bSave)
	{
		// Set the default "filename"
		if (!DefaultFile.IsEmpty())
		{
			FileDialog->SetFileName(*FPaths::GetCleanFilename(DefaultFile));
		}
	}
	else
	{
		// Set this up as a multi-select picker
		if (bAllowMultiple)
		{
			DWORD dwFlags = 0;
			FileDialog->GetOptions(&dwFlags);
			FileDialog->SetOptions(dwFlags | FOS_ALLOWMULTISELECT);
		}
	}

	// Set up common settings
	FileDialog->SetTitle(*DialogTitle);
	if (!DefaultPath.IsEmpty())
	{
		// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
		FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
		DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

		TComPtr<IShellItem> DefaultPathItem;
		if (SUCCEEDED(SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
		{
			FileDialog->SetFolder(DefaultPathItem);
		}
	}

	// Set-up the file type filters
	TArray<FString> UnformattedExtensions;
	TArray<COMDLG_FILTERSPEC> FileDialogFilters;
	{
		// Split the given filter string (formatted as "Pair1String1|Pair1String2|Pair2String1|Pair2String2") into the Windows specific filter struct
		FileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

		if (UnformattedExtensions.Num() % 2 == 0)
		{
			FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
			for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
			{
				COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
				NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
				NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
			}
		}
	}
	FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

	// Show the picker
	if (!SUCCEEDED(FileDialog->Show(NULL)))
	{
		return false;
	}

	int32 OutFilterIndex = 0;
	if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
	{
		OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
	}

	TFunction<void(const FString&)> AddOutFilename = [&OutFilenames](const FString& InFilename)
	{
		FString& OutFilename = OutFilenames.Add_GetRef(InFilename);
		OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
		FPaths::NormalizeFilename(OutFilename);
	};

	if (bSave)
	{
		TComPtr<IShellItem> Result;
		if (!SUCCEEDED(FileDialog->GetResult(&Result)))
		{
			return false;
		}

		PWSTR pFilePath = nullptr;
		if (!SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
		{
			return false;
		}

		// Apply the selected extension if the given filename doesn't already have one
		FString SaveFilePath = pFilePath;
		if (FileDialogFilters.IsValidIndex(OutFilterIndex))
		{
			// May have multiple semi-colon separated extensions in the pattern
			const FString ExtensionPattern = FileDialogFilters[OutFilterIndex].pszSpec;
			TArray<FString> SaveExtensions;
			ExtensionPattern.ParseIntoArray(SaveExtensions, TEXT(";"));

			// Build a "clean" version of the selected extension (without the wildcard)
			FString CleanExtension = SaveExtensions[0];
			if (CleanExtension == TEXT("*.*"))
			{
				CleanExtension.Reset();
			}
			else
			{
				int32 WildCardIndex = INDEX_NONE;
				if (CleanExtension.FindChar(TEXT('*'), WildCardIndex))
				{
					CleanExtension.RightChopInline(WildCardIndex + 1, false);
				}
			}

			// We need to split these before testing the extension to avoid anything within the path being treated as a file extension
			FString SaveFileName = FPaths::GetCleanFilename(SaveFilePath);
			SaveFilePath = FPaths::GetPath(SaveFilePath);

			// Apply the extension if the file name doesn't already have one
			if (FPaths::GetExtension(SaveFileName).IsEmpty() && !CleanExtension.IsEmpty())
			{
				SaveFileName = FPaths::SetExtension(SaveFileName, CleanExtension);
			}

			SaveFilePath /= SaveFileName;
		}
		AddOutFilename(SaveFilePath);

		CoTaskMemFree(pFilePath);

		return true;
	}
	else
	{
		IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

		TComPtr<IShellItemArray> Results;
		if (!SUCCEEDED(FileOpenDialog->GetResults(&Results)))
		{
			return false;
		}

		DWORD NumResults = 0;
		Results->GetCount(&NumResults);
		for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
		{
			TComPtr<IShellItem> Result;
			if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
			{
				PWSTR pFilePath = nullptr;
				if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
				{
					AddOutFilename(pFilePath);
					CoTaskMemFree(pFilePath);
				}
			}
		}

		return true;
	}
#endif

	ensure(false);
	return false;
}