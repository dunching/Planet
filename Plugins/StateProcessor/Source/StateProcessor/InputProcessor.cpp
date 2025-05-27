
#include "InputProcessor.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

FInputProcessor::FInputProcessor(FOwnerPawnType* CharacterPtr) :
                                                               OnwerPawnPtr(CharacterPtr)
{

}

FInputProcessor::~FInputProcessor()
{

}

void FInputProcessor::TickImp(float Delta)
{

}

void FInputProcessor::SwitchShowCursor(
	bool bIsShowCursor
)
{
	auto OnwerActorPtr = GetOwnerActor<FOwnerPawnType>();
	if (OnwerActorPtr)
	{
		auto PlayerPCPtr = OnwerActorPtr->GetController<APlayerController>();
		if (PlayerPCPtr)
		{
			if (bIsShowCursor)
			{
				PlayerPCPtr->SetShowMouseCursor(true);

				UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerPCPtr, nullptr, EMouseLockMode::DoNotLock, true);
			}
			else
			{
				PlayerPCPtr->SetShowMouseCursor(false);

				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerPCPtr);
			}
		}
	}
}
bool FInputProcessor::Tick(float Delta)
{
	IncreaseAsyncTaskNum();

	TickImp(Delta);

	ReduceAsyncTaskNum();

#if WITH_EDITOR
	return !bIsRequestQuit;
#else
	return !bIsRequestQuit
#endif
}

void FInputProcessor::EnterAction()
{
	bIsRequestQuit = false;

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &ThisClass::Tick), Frequency);
}

void FInputProcessor::ReEnterAction()
{

}

void FInputProcessor::QuitAction()
{
	if (OnQuitFunc)
	{
		OnQuitFunc();
	}
	
	UnRegisterTicker();
}

void FInputProcessor::BeginDestroy()
{

}

bool FInputProcessor::InputKey(
	const FInputKeyEventArgs& EventArgs
)
{
	return true;
}

bool FInputProcessor::InputAxis(
	FViewport* Viewport,
	FInputDeviceId InputDevice,
	FKey Key,
	float Delta,
	float DeltaTime,
	int32 NumSamples,
	bool bGamepad
)
{
	return true;
}

bool FInputProcessor::GetIsComplete() const
{
    return AsyncTaskNum == 0;
}

void FInputProcessor::SetPawn(FOwnerPawnType* NewPawnPtr)
{
	OnwerPawnPtr = NewPawnPtr;
}

void FInputProcessor::UnRegisterTicker()
{
	bIsRequestQuit = true;

	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void FInputProcessor::IncreaseAsyncTaskNum()
{
	AsyncTaskNum++;
}

void FInputProcessor::ReduceAsyncTaskNum()
{
	if (AsyncTaskNum > 0)
	{
		AsyncTaskNum--;
	}
}

bool FInputProcessor::GetIsRequestQuit() const
{
	return bIsRequestQuit.load();
}
