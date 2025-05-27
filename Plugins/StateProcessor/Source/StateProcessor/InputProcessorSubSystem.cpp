#include "InputProcessorSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include <GameDelegates.h>

#include "LogWriter.h"

#include "InputProcessor.h"

void UInputProcessorSubSystem::Initialize(
	FSubsystemCollectionBase& Collection
)
{
	Super::Initialize(Collection);

	CurrentProcessorSPtr = MakeShared<FInputProcessor>(nullptr);

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &ThisClass::Tick),
		Frequency
	);

#if WITH_EDITOR
	FGameDelegates::Get().GetExitCommandDelegate().AddLambda(
		[]()
		{
			PRINTINVOKEINFO();
		}
	);
	FGameDelegates::Get().GetEndPlayMapDelegate().AddLambda(
		[]()
		{
			PRINTINVOKEINFO();
		}
	);
	FCoreDelegates::OnPreExit.AddLambda(
		[]()
		{
			PRINTINVOKEINFO();
		}
	);
	FCoreDelegates::OnEnginePreExit.AddLambda(
		[]()
		{
			PRINTINVOKEINFO();
		}
	);
	FWorldDelegates::OnWorldBeginTearDown.AddLambda(
		[](
		UWorld*

	
	)
		{
			PRINTINVOKEINFO();
		}
	);
#else
#endif
}

void UInputProcessorSubSystem::Deinitialize()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	Super::Deinitialize();
}

UInputProcessorSubSystem* UInputProcessorSubSystem::GetInstance()
{
	return Cast<UInputProcessorSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UInputProcessorSubSystem::StaticClass())
	);
}

TSharedPtr<FInputProcessor>& UInputProcessorSubSystem::GetCurrentAction()
{
	return CurrentProcessorSPtr;
}

bool UInputProcessorSubSystem::Tick(
	float DeltaTime
)
{
	decltype(ActionCacheSet) NeedRemoveSet;
	for (auto Iter : ActionCacheSet)
	{
		if (Iter.IsValid())
		{
			if (Iter->GetIsComplete() && Iter->GetIsRequestQuit())
			{
				Iter->BeginDestroy();
				NeedRemoveSet.Add(Iter);
			}
		}
		else
		{
			NeedRemoveSet.Add(Iter);
		}
	}

	for (auto Iter : NeedRemoveSet)
	{
		ActionCacheSet.Remove(Iter);
	}

	return true;
}

void UInputProcessorSubSystem::ResetProcessor()
{
	if (CurrentProcessorSPtr)
	{
		CurrentProcessorSPtr->QuitAction();
	}
}

FDelegateHandle UInputProcessorSubSystem::AddKeyEvent(
	FKey Key,
	const std::function<void(
		EInputEvent
	)>& KeyEvent
)
{
	if (OnKeyPressedMap.Contains(Key))
	{
		return OnKeyPressedMap[Key].AddLambda(KeyEvent);
	}
	else
	{
		FSYKeyEvent SYKeyEvent;
		auto Delegate = SYKeyEvent.AddLambda(KeyEvent);
		OnKeyPressedMap.Add(Key, SYKeyEvent);
		return Delegate;
	}
}

void UInputProcessorSubSystem::RemoveKeyEvent(
	FDelegateHandle DelegateHandle
)
{
	for (auto& Iter : OnKeyPressedMap)
	{
		Iter.Value.Remove(DelegateHandle);
	}
}

bool UInputProcessorSubSystem::InputKey(
	const FInputKeyEventArgs& EventArgs
)
{
	if (CurrentProcessorSPtr)
	{
		return CurrentProcessorSPtr->InputKey(EventArgs);
	}
	else
	{
		return true;
	}
}

bool UInputProcessorSubSystem::InputAxis(
	FViewport* Viewport,
	FInputDeviceId InputDevice,
	FKey Key,
	float Delta,
	float DeltaTime,
	int32 NumSamples,
	bool bGamepad
)
{
	if (CurrentProcessorSPtr)
	{
		return CurrentProcessorSPtr->InputAxis(Viewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}
	else
	{
		return true;
	}
}
