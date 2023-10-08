// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.h"
#include "VoxelActor.h"
#include "VoxelSettings.h"
#include "VoxelComponent.h"
#include "VoxelGraphExecutor.h"
#include "VoxelParameterContainer.h"
#include "Material/VoxelMaterialDefinition.h"
#include "FunctionLibrary/VoxelParameterFunctionLibrary.h"
#if WITH_EDITOR
#include "MessageLogModule.h"
#include "IMessageLogListing.h"
#include "Editor/EditorEngine.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#endif

TSet<FObjectKey> GVoxelObjectsDestroyedByFrameRateLimit;

VOXEL_CONSOLE_COMMAND(
	RefreshAll,
	"voxel.RefreshAll",
	"")
{
#if WITH_EDITOR
	if (GEditor)
	{
		FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog").GetLogListing("Voxel")->ClearMessages();
	}

	for (IVoxelNodeStatProvider* Provider : GVoxelNodeStatProviders)
	{
		Provider->ClearStats();
	}
#endif

	GVoxelGraphExecutorManager->RecompileAll();

	ForEachObjectOfClass<AVoxelActor>([&](AVoxelActor* Actor)
	{
		if (Actor->IsRuntimeCreated() ||
			GVoxelObjectsDestroyedByFrameRateLimit.Contains(Actor))
		{
			Actor->QueueRecreate();
		}
	});

	ForEachObjectOfClass<UVoxelComponent>([&](UVoxelComponent* Component)
	{
		if (Component->IsRuntimeCreated() ||
			GVoxelObjectsDestroyedByFrameRateLimit.Contains(Component))
		{
			Component->QueueRecreate();
		}
	});

	ForEachObjectOfClass<UVoxelMaterialDefinition>([&](UVoxelMaterialDefinition* Definition)
	{
		Definition->QueueRebuildTextures();
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static TArray<FString> ParseArguments(const TArray<FString>& Args)
{
	TArray<FString> Result;

	FString CurrentArg;
	for (int32 Index = 0; Index < Args.Num(); Index++)
	{
		if (Args[Index].StartsWith("\""))
		{
			CurrentArg = Args[Index];
			CurrentArg.RemoveFromStart("\"");

			if (CurrentArg.EndsWith("\""))
			{
				CurrentArg.RemoveFromEnd("\"");
				Result.Add(CurrentArg);
			}
			continue;
		}

		if (CurrentArg.IsEmpty())
		{
			Result.Add(Args[Index]);
			continue;
		}

		if (!Args[Index].EndsWith("\""))
		{
			CurrentArg += " " + Args[Index];
			continue;
		}

		CurrentArg += " " + Args[Index];
		CurrentArg.RemoveFromEnd("\"");
		Result.Add(CurrentArg);
		CurrentArg = {};
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

VOXEL_CONSOLE_WORLD_COMMAND(
	GetParameter,
	"voxel.get",
	"[ActorName] [ParameterName] Gets graph parameter value from specific actor if specified, all voxel actors otherwise. Actor name is optional.")
{
	if (Args.Num() == 0)
	{
		UE_LOG(LogConsoleResponse, Warning, TEXT("Missing parameter name argument"));
		return;
	}

	TArray<FString> ParsedArgs = ParseArguments(Args);

	FString ActorName;
	FName ParameterName;
	if (ParsedArgs.Num() > 1)
	{
		ActorName = ParsedArgs[0];
		ParameterName = FName(ParsedArgs[1]);
	}
	else
	{
		ParameterName = FName(ParsedArgs[0]);
	}

	if (ActorName.IsEmpty())
	{
		for (TActorIterator<AVoxelActor> It(World, AVoxelActor::StaticClass()); It; ++It)
		{
			const AVoxelActor* Actor = *It;
			if (!ensure(Actor))
			{
				continue;
			}

			if (!UVoxelParameterFunctionLibrary::HasVoxelParameter(Actor->ParameterContainer, ParameterName))
			{
				continue;
			}

			FVoxelPinValue Value = UVoxelParameterFunctionLibrary::GetVoxelParameter(Actor->ParameterContainer, ParameterName);
			if (!Value.IsValid())
			{
				continue;
			}

			UE_LOG(LogConsoleResponse, Log, TEXT("%s.%s=%s"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString(), *Value.ExportToString());
		}
		return;
	}

	for (TActorIterator<AVoxelActor> It(World, AVoxelActor::StaticClass()); It; ++It)
	{
		const AVoxelActor* Actor = *It;
		if (!ensure(Actor) ||
			Actor->GetActorNameOrLabel() != ActorName)
		{
			continue;
		}

		if (!UVoxelParameterFunctionLibrary::HasVoxelParameter(Actor->ParameterContainer, ParameterName))
		{
			UE_LOG(LogConsoleResponse, Warning, TEXT("%s: No parameter %s found"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString());
			return;
		}

		const FVoxelPinValue Value = UVoxelParameterFunctionLibrary::GetVoxelParameter(Actor->ParameterContainer, ParameterName);
		if (!Value.IsValid())
		{
			return;
		}

		UE_LOG(LogConsoleResponse, Log, TEXT("%s.%s=%s"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString(), *Value.ExportToString());
		return;
	}

	UE_LOG(LogConsoleResponse, Warning, TEXT("No actor %s found"), *ActorName);
}

VOXEL_CONSOLE_WORLD_COMMAND(
	SetParameter,
	"voxel.set",
	"[ActorName] [ParameterName] [Value] Sets graph parameter value for either specific voxel actor if specified, otherwise all voxel actors. Actor name is optional.")
{
	TArray<FString> ParsedArgs = ParseArguments(Args);
	if (ParsedArgs.Num() < 2)
	{
		UE_LOG(LogConsoleResponse, Warning, TEXT("Missing arguments"));
		return;
	}

	FString ActorName;
	FName ParameterName;
	FString ParameterValue;
	if (ParsedArgs.Num() > 2)
	{
		ActorName = ParsedArgs[0];
		ParameterName = FName(ParsedArgs[1]);
		ParameterValue = ParsedArgs[2];
	}
	else
	{
		ParameterName = FName(ParsedArgs[0]);
		ParameterValue = ParsedArgs[1];
	}

	if (ActorName.IsEmpty())
	{
		for (TActorIterator<AVoxelActor> It(World, AVoxelActor::StaticClass()); It; ++It)
		{
			const AVoxelActor* Actor = *It;
			if (!ensure(Actor))
			{
				continue;
			}

			if (!UVoxelParameterFunctionLibrary::HasVoxelParameter(Actor->ParameterContainer, ParameterName))
			{
				continue;
			}

			FVoxelPinValue Value = UVoxelParameterFunctionLibrary::GetVoxelParameter(Actor->ParameterContainer, ParameterName);
			if (!Value.IsValid())
			{
				continue;
			}

			if (!Value.ImportFromString(ParameterValue))
			{
				UE_LOG(LogConsoleResponse, Warning, TEXT("%s: Failed to set %s=%s"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString(), *ParameterValue);
				continue;
			}

			FString Error;
			if (!Actor->ParameterContainer->Set(ParameterName, Value, &Error))
			{
				UE_LOG(LogConsoleResponse, Warning, TEXT("%s: %s"), *Actor->GetActorNameOrLabel(), *Error);
				continue;
			}

			UE_LOG(LogConsoleResponse, Log, TEXT("%s.%s=%s"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString(), *Value.ExportToString());
		}

		return;
	}

	for (TActorIterator<AVoxelActor> It(World, AVoxelActor::StaticClass()); It; ++It)
	{
		const AVoxelActor* Actor = *It;
		if (!ensure(Actor) ||
			Actor->GetActorNameOrLabel() != ActorName)
		{
			continue;
		}

		if (!UVoxelParameterFunctionLibrary::HasVoxelParameter(Actor->ParameterContainer, ParameterName))
		{
			UE_LOG(LogConsoleResponse, Warning, TEXT("%s: No parameter %s found"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString());
			return;
		}

		FVoxelPinValue Value = UVoxelParameterFunctionLibrary::GetVoxelParameter(Actor->ParameterContainer, ParameterName);
		if (!Value.IsValid())
		{
			return;
		}

		if (!Value.ImportFromString(ParameterValue))
		{
			UE_LOG(LogConsoleResponse, Warning, TEXT("%s: Failed to set %s=%s"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString(), *ParameterValue);
			return;
		}

		FString Error;
		if (!Actor->ParameterContainer->Set(ParameterName, Value, &Error))
		{
			UE_LOG(LogConsoleResponse, Warning, TEXT("%s: %s"), *Actor->GetActorNameOrLabel(), *Error);
			return;
		}

		UE_LOG(LogConsoleResponse, Log, TEXT("%s.%s=%s"), *Actor->GetActorNameOrLabel(), *ParameterName.ToString(), *Value.ExportToString());
		return;
	}

	UE_LOG(LogConsoleResponse, Warning, TEXT("No actor %s found"), *ActorName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
// https://mveg.es/posts/fast-numerically-stable-moving-average/
struct FVoxelAverageDoubleBuffer
{
	explicit FVoxelAverageDoubleBuffer(const int32 ValuesCount)
		: WindowSize(1 << FMath::CeilLogTwo(ValuesCount))
	{
		ensure(FMath::IsPowerOfTwo(ValuesCount));
		FVoxelUtilities::SetNum(Values, WindowSize * 2);
	}

	void AddValue(const double NewValue)
	{
		const int32 PositionIndex = WindowSize - 1 + Position;
		Position = (Position + 1) % WindowSize;

		Values[PositionIndex] = NewValue;

		// Update parents
		for (int32 Index = PositionIndex; Index > 0; Index = GetParentIndex(Index))
		{
			const int32 ParentToUpdate = GetParentIndex(Index);
			Values[ParentToUpdate] = Values[GetLeftChildIndex(ParentToUpdate)] + Values[GetRightChildIndex(ParentToUpdate)];
		}
	}

	FORCEINLINE double GetAverageValue() const
	{
		return Values[0] / WindowSize;
	}

	FORCEINLINE int32 GetWindowSize() const
	{
		return WindowSize;
	}

private:
	static int32 GetParentIndex(const int32 ChildIndex)
	{
		check(ChildIndex > 0);
		return (ChildIndex - 1) / 2;
	}

	static int32 GetLeftChildIndex(const int32 ParentIndex)
	{
		return 2 * ParentIndex + 1;
	}

	static int32 GetRightChildIndex(const int32 ParentIndex)
	{
		return 2 * ParentIndex + 2;
	}

private:
	int32 WindowSize = 0;
	int32 Position = 0;
	TArray<double> Values;
};

class FVoxelSafetyTicker : public FVoxelTicker
{
public:
	//~ Begin FVoxelTicker Interface
	virtual void Tick() override
	{
		VOXEL_FUNCTION_COUNTER();

		if (!GetDefault<UVoxelSettings>()->bEnablePerformanceMonitoring)
		{
			return;
		}

		if (GEditor->ShouldThrottleCPUUsage() ||
			GEditor->PlayWorld ||
			GIsPlayInEditorWorld)
		{
			// Don't check framerate when throttling or in PIE
			return;
		}

		const int32 FramesToAverage = FMath::Max(2, GetDefault<UVoxelSettings>()->FramesToAverage);
		if (FramesToAverage != Buffer.GetWindowSize())
		{
			Buffer = FVoxelAverageDoubleBuffer(FramesToAverage);
		}

		// Avoid outliers (typically, debugger breaking) causing a huge average
		const double SanitizedDeltaTime = FMath::Clamp(FApp::GetDeltaTime(), 0.001, 1);
		Buffer.AddValue(SanitizedDeltaTime);

		if (1.f / Buffer.GetAverageValue() > GetDefault<UVoxelSettings>()->MinFPS)
		{
			bDestroyedRuntimes = false;
			return;
		}

		if (bDestroyedRuntimes)
		{
			return;
		}
		bDestroyedRuntimes = true;

		FNotificationInfo Info(INVTEXT("Average framerate is below 8fps, destroying all voxel runtimes. Use Ctrl F5 to re-create them"));
		Info.ExpireDuration = 4.f;
		Info.ButtonDetails.Add(FNotificationButtonInfo(
			INVTEXT("Disable Monitoring"),
			INVTEXT("Disable framerate monitoring"),
			MakeLambdaDelegate([]
			{
				GetMutableDefault<UVoxelSettings>()->bEnablePerformanceMonitoring = false;
				GetMutableDefault<UVoxelSettings>()->PostEditChange();
			}),
			SNotificationItem::CS_None));
		FSlateNotificationManager::Get().AddNotification(Info);

		ForEachObjectOfClass_Copy<AVoxelActor>([&](AVoxelActor* Actor)
		{
			if (!Actor->IsRuntimeCreated())
			{
				return;
			}

			if (!ensure(Actor->GetWorld()) ||
				!Actor->GetWorld()->IsEditorWorld())
			{
				return;
			}

			Actor->DestroyRuntime();
			GVoxelObjectsDestroyedByFrameRateLimit.Add(Actor);
		});

		ForEachObjectOfClass_Copy<UVoxelComponent>([&](UVoxelComponent* Component)
		{
			if (!Component->IsRuntimeCreated())
			{
				return;
			}

			if (!ensure(Component->GetWorld()) ||
				!Component->GetWorld()->IsEditorWorld())
			{
				return;
			}

			Component->DestroyRuntime();
			GVoxelObjectsDestroyedByFrameRateLimit.Add(Component);
		});
	}
	//~ End FVoxelTicker Interface

private:
	FVoxelAverageDoubleBuffer Buffer = FVoxelAverageDoubleBuffer(2);
	bool bDestroyedRuntimes = false;
};

VOXEL_RUN_ON_STARTUP_EDITOR(CreateVoxelSafetyTicker)
{
	if (!GEditor)
	{
		return;
	}

	new FVoxelSafetyTicker();
}
#endif