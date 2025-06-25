#include "InputProcessor.h"

#include "InputProcessorSubSystemBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

FInputProcessor::FInputProcessor(
	FOwnerPawnType* CharacterPtr
	) :
	  OnwerPawnPtr(CharacterPtr)
{
}

FInputProcessor::~FInputProcessor()
{
}

void FInputProcessor::TickImp(
	float Delta
	)
{
}

void FInputProcessor::SwitchShowCursor(
	bool bIsShowCursor
	)
{
	UInputProcessorSubSystemBase::GetInstanceBase()->SwitchShowCursor(bIsShowCursor);
}

bool FInputProcessor::Tick(
	float Delta
	)
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

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(
	                                                          FTickerDelegate::CreateRaw(this, &ThisClass::Tick),
	                                                          Frequency
	                                                         );
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

void FInputProcessor::GameDown()
{
	UnRegisterTicker();
}

bool FInputProcessor::InputKey(
	const FInputKeyEventArgs& EventArgs
	)
{
	return true;
}

bool FInputProcessor::InputAxis(
	const FInputKeyEventArgs& EventArgs
	)
{
	return true;
}

bool FInputProcessor::GetIsComplete() const
{
	return AsyncTaskNum == 0;
}

void FInputProcessor::SetPawn(
	FOwnerPawnType* NewPawnPtr
	)
{
	OnwerPawnPtr = NewPawnPtr;
}

void FInputProcessor::UnRegisterTicker()
{
	bIsRequestQuit = true;

	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void FInputProcessor::SetOnQuitFunc(
	const FOnQuitFunc& InOnQuitFunc
	)
{
	OnQuitFunc = InOnQuitFunc;
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
