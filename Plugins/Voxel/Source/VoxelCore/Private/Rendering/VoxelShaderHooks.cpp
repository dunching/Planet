// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Rendering/VoxelShaderHooks.h"
#include "ShaderCore.h"
#include "HAL/PlatformFileManager.h"

#if WITH_EDITOR
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlOperations.h"
#endif

#if WITH_EDITOR
struct FVoxelShaderHookStatics
{
public:
	bool bApplied = false;
	TMap<EVoxelShaderHookLocation, TArray<FString>> LocationToHooks;

	static FVoxelShaderHookStatics& Get()
	{
		static FVoxelShaderHookStatics Statics;
		return Statics;
	}

	void Apply()
	{
		VOXEL_FUNCTION_COUNTER();
		ON_SCOPE_EXIT
		{
			LocationToHooks.Empty();
		};

		check(!bApplied);
		bApplied = true;

		FString Path = GetShaderSourceFilePath("/Engine/Private/MaterialTemplate.ush");
		if (!ensure(!Path.IsEmpty()))
		{
			return;
		}
		Path = FPaths::ConvertRelativePathToFull(Path);

		FString MaterialTemplate;
		if (!ensure(FFileHelper::LoadFileToString(MaterialTemplate, *Path)))
		{
			return;
		}

		bool bWrite = false;
		bWrite |= ApplyVertexHooks(MaterialTemplate);
		bWrite |= ApplyPixelHooks(MaterialTemplate);

		if (!bWrite)
		{
			return;
		}

		if (IFileManager::Get().IsReadOnly(*Path))
		{
			INLINE_LAMBDA
			{
				ISourceControlProvider & Provider = ISourceControlModule::Get().GetProvider();
				if (!Provider.IsEnabled())
				{
					VOXEL_MESSAGE(Warning, "{0} is readonly but no source control provider: manually setting it to not-readonly", Path);
					return;
				}

				const TSharedPtr<ISourceControlState> State = Provider.GetState(*Path, EStateCacheUsage::ForceUpdate);
				if (!State)
				{
					VOXEL_MESSAGE(Warning, "{0} is readonly but failed to get source control state: manually setting it to not-readonly", Path);
					return;
				}
				if (!State->IsSourceControlled())
				{
					VOXEL_MESSAGE(Warning, "{0} is readonly but file is not source controlled: manually setting it to not-readonly", Path);
					return;
				}

				if (State->IsCheckedOut())
				{
					return;
				}

				if (!State->CanCheckout())
				{
					VOXEL_MESSAGE(Warning, "{0} is readonly but file cannot be checked out: manually setting it to not-readonly", Path);
					return;
				}

				TArray<FString> FilesToBeCheckedOut;
				FilesToBeCheckedOut.Add(Path);
				if (Provider.Execute(ISourceControlOperation::Create<FCheckOut>(), FilesToBeCheckedOut) == ECommandResult::Succeeded)
				{
					VOXEL_MESSAGE(Warning, "{0} is readonly but file failed to be checked out: manually setting it to not-readonly", Path);
					return;
				}

				VOXEL_MESSAGE(Info, "{0} checked out", Path);

				if (IFileManager::Get().IsReadOnly(*Path))
				{
					VOXEL_MESSAGE(Warning, "{0} is readonly after check out: manually setting it to not-readonly", Path);
					return;
				}

				ensure(!IFileManager::Get().IsReadOnly(*Path));
			};
		}

		if (IFileManager::Get().IsReadOnly(*Path))
		{
			if (!ensure(FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*Path, false)))
			{
				VOXEL_MESSAGE(Error, "Cannot clear readonly flag on {0}", Path);
			}
		}

		if (!FFileHelper::SaveStringToFile(MaterialTemplate, *Path))
		{
			VOXEL_MESSAGE(Error, "Failed to write {0}", Path);
		}

		VOXEL_MESSAGE(Info, "{0} updated by Voxel Plugin", Path);
	}

private:
	static bool Parse(const FString& MaterialTemplate, int32& Index, const FString& Text)
	{
		int32 LocalIndex = Index;
		int32 TextIndex = 0;
		while (TextIndex < Text.Len())
		{
			if (!ensure(LocalIndex < MaterialTemplate.Len()))
			{
				return false;
			}
			if (MaterialTemplate[LocalIndex] == Text[TextIndex])
			{
				LocalIndex++;
				TextIndex++;
				continue;
			}

			if (FChar::IsWhitespace(MaterialTemplate[LocalIndex]))
			{
				LocalIndex++;
				continue;
			}

			return false;
		}

		Index = LocalIndex;
		ensure(TextIndex == Text.Len());
		return true;
	}
	bool ApplyVertexHooks(FString& MaterialTemplate)
	{
		VOXEL_FUNCTION_COUNTER();

		TArray<FString> Hooks = LocationToHooks.FindOrAdd(EVoxelShaderHookLocation::MaterialVertexParameters);
		Hooks.Sort();

		TArray<FString> LinesToAdd;
		for (const FString& Hook : Hooks)
		{
			TArray<FString> Lines;
			Hook.ParseIntoArrayLines(Lines, false);

			for (FString& Line : Lines)
			{
				Line.TrimStartAndEndInline();
				if (Line.IsEmpty())
				{
					continue;
				}

				if (!Line.StartsWith("#"))
				{
					Line = TEXT("\t") + Line;
				}
				LinesToAdd.Add(Line);
			}
		}

		int32 Index = MaterialTemplate.Find(TEXT("#if WATER_MESH_FACTORY"), ESearchCase::CaseSensitive);
		if (!ensure(Index != -1))
		{
			return false;
		}

		// Find second one, first one is pixel
		Index = MaterialTemplate.Find(TEXT("#if WATER_MESH_FACTORY"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Index + 1);
		if (!ensure(Index != -1))
		{
			return false;
		}

		if (!ensure(Parse(MaterialTemplate, Index, "#if WATER_MESH_FACTORY"))) return false;
		if (!ensure(Parse(MaterialTemplate, Index, "uint WaterWaveParamIndex;"))) return false;
		if (!ensure(Parse(MaterialTemplate, Index, "#endif"))) return false;

		const int32 InsertIndex = Index;

		if (Parse(MaterialTemplate, Index, "// BEGIN VOXEL SHADER"))
		{
			const FString EndVoxelShader = "// END VOXEL SHADER";
			const int32 EndIndex = MaterialTemplate.Find(
				EndVoxelShader,
				ESearchCase::CaseSensitive,
				ESearchDir::FromStart,
				Index);
			if (!ensure(EndIndex != -1))
			{
				return false;
			}

			const FString VoxelText = MaterialTemplate.Mid(Index, EndIndex - Index);

			if (!ensure(!VoxelText.Contains(TEXT("}"))))
			{
				return false;
			}

			TArray<FString> ParsedLines;
			VoxelText.ParseIntoArrayLines(ParsedLines, false);

			TArray<FString> Lines;
			for (FString& Line : ParsedLines)
			{
				Line.TrimStartAndEndInline();
				if (Line.IsEmpty())
				{
					continue;
				}

				if (!Line.StartsWith("#"))
				{
					Line = TEXT("\t") + Line;
				}
				Lines.Add(Line);
			}

			if (Lines == LinesToAdd)
			{
				return false;
			}

			int32 Count = EndIndex - InsertIndex + EndVoxelShader.Len();
			if (MaterialTemplate[InsertIndex + Count] == TEXT('\r'))
			{
				Count++;
			}
			if (MaterialTemplate[InsertIndex + Count] == TEXT('\n'))
			{
				Count++;
			}

			MaterialTemplate.RemoveAt(InsertIndex, Count);
		}

		Index = InsertIndex;

		if (!ensure(Parse(MaterialTemplate, Index, "FMaterialAttributes MaterialAttributes;")))
		{
			return false;
		}

		LinesToAdd.Insert("", 0);
		LinesToAdd.Insert("// BEGIN VOXEL SHADER", 1);
		LinesToAdd.Add("// END VOXEL SHADER");
		LinesToAdd.Add("");

		FString MergedLines;
		if (MaterialTemplate.Contains(TEXT("\r\n")))
		{
			MergedLines = FString::Join(LinesToAdd, TEXT("\r\n"));
		}
		else
		{
			MergedLines = FString::Join(LinesToAdd, TEXT("\n"));
		}

		MaterialTemplate.InsertAt(InsertIndex, MergedLines);
		return true;
	}
	bool ApplyPixelHooks(FString& MaterialTemplate)
	{
		VOXEL_FUNCTION_COUNTER();

		TArray<FString> Hooks = LocationToHooks.FindOrAdd(EVoxelShaderHookLocation::MaterialPixelParameters);
		Hooks.Sort();

		TArray<FString> LinesToAdd;
		for (const FString& Hook : Hooks)
		{
			TArray<FString> Lines;
			Hook.ParseIntoArrayLines(Lines, false);

			for (FString& Line : Lines)
			{
				Line.TrimStartAndEndInline();
				if (Line.IsEmpty())
				{
					continue;
				}

				if (!Line.StartsWith("#"))
				{
					Line = TEXT("\t") + Line;
				}
				LinesToAdd.Add(Line);
			}
		}

		int32 Index = MaterialTemplate.Find(TEXT("#if WATER_MESH_FACTORY"), ESearchCase::CaseSensitive);
		if (!ensure(Index != -1))
		{
			return false;
		}

		if (!ensure(Parse(MaterialTemplate, Index, "#if WATER_MESH_FACTORY"))) return false;
		if (!ensure(Parse(MaterialTemplate, Index, "uint WaterWaveParamIndex;"))) return false;
		if (!ensure(Parse(MaterialTemplate, Index, "#endif"))) return false;

		const int32 InsertIndex = Index;

		if (Parse(MaterialTemplate, Index, "// BEGIN VOXEL SHADER"))
		{
			const FString EndVoxelShader = "// END VOXEL SHADER";
			const int32 EndIndex = MaterialTemplate.Find(
				EndVoxelShader,
				ESearchCase::CaseSensitive,
				ESearchDir::FromStart,
				Index);
			if (!ensure(EndIndex != -1))
			{
				return false;
			}

			const FString VoxelText = MaterialTemplate.Mid(Index, EndIndex - Index);

			if (!ensure(!VoxelText.Contains(TEXT("}"))))
			{
				return false;
			}

			TArray<FString> ParsedLines;
			VoxelText.ParseIntoArrayLines(ParsedLines, false);

			TArray<FString> Lines;
			for (FString& Line : ParsedLines)
			{
				Line.TrimStartAndEndInline();
				if (Line.IsEmpty())
				{
					continue;
				}

				if (!Line.StartsWith("#"))
				{
					Line = TEXT("\t") + Line;
				}
				Lines.Add(Line);
			}

			if (Lines == LinesToAdd)
			{
				return false;
			}

			int32 Count = EndIndex - InsertIndex + EndVoxelShader.Len();
			if (MaterialTemplate[InsertIndex + Count] == TEXT('\r'))
			{
				Count++;
			}
			if (MaterialTemplate[InsertIndex + Count] == TEXT('\n'))
			{
				Count++;
			}

			MaterialTemplate.RemoveAt(InsertIndex, Count);
		}

		Index = InsertIndex;

		if (!ensure(Parse(MaterialTemplate, Index, "#if CLOUD_LAYER_PIXEL_SHADER")))
		{
			return false;
		}

		LinesToAdd.Insert("", 0);
		LinesToAdd.Insert("// BEGIN VOXEL SHADER", 1);
		LinesToAdd.Add("// END VOXEL SHADER");
		LinesToAdd.Add("");

		FString MergedLines;
		if (MaterialTemplate.Contains(TEXT("\r\n")))
		{
			MergedLines = FString::Join(LinesToAdd, TEXT("\r\n"));
		}
		else
		{
			MergedLines = FString::Join(LinesToAdd, TEXT("\n"));
		}

		MaterialTemplate.InsertAt(InsertIndex, MergedLines);
		return true;
	}
};

void RegisterVoxelShaderHook(const EVoxelShaderHookLocation Location, const FString& Text)
{
	check(!FVoxelShaderHookStatics::Get().bApplied);
	FVoxelShaderHookStatics::Get().LocationToHooks.FindOrAdd(Location).Add(Text);
}

VOXEL_RUN_ON_STARTUP_EDITOR(ApplyVoxelShaderHooks)
{
	FVoxelShaderHookStatics::Get().Apply();
}
#endif