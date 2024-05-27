
#include "InputProcessor.h"

#include "Animation/AnimInstanceBase.h"

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

bool FInputProcessor::Tick(float Delta)
{
	IncreaseAsyncTaskNum();

	TickImp(Delta);

	ReduceAsyncTaskNum();

	return !bIsRequestQuit;
}

void FInputProcessor::ESCKeyPressed()
{
}

void FInputProcessor::TabKeyPressed()
{

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
	bIsRequestQuit = true;

	FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void FInputProcessor::BeginDestroy()
{

}

void FInputProcessor::InputKey(const FInputKeyParams& Params)
{

}

void FInputProcessor::AKeyPressed()
{

}

void FInputProcessor::AKeyReleased()
{

}

void FInputProcessor::SKeyPressed()
{

}

void FInputProcessor::SKeyReleased()
{

}

void FInputProcessor::DKeyPressed()
{

}

void FInputProcessor::DKeyReleased()
{

}

void FInputProcessor::QKeyPressed()
{

}

void FInputProcessor::WKeyPressed()
{

}

void FInputProcessor::WKeyReleased()
{

}

void FInputProcessor::EKeyPressed()
{

}

void FInputProcessor::EKeyReleased()
{

}

void FInputProcessor::RKeyPressed()
{

}

void FInputProcessor::TKeyPressed()
{

}

void FInputProcessor::YKeyPressed()
{

}

void FInputProcessor::BKeyPressed()
{

}

void FInputProcessor::FKeyPressed()
{
}

void FInputProcessor::XKeyPressed()
{

}

void FInputProcessor::CKeyPressed()
{

}

void FInputProcessor::VKeyPressed()
{

}

void FInputProcessor::GKeyPressed()
{
}

void FInputProcessor::HKeyPressed()
{

}

void FInputProcessor::LCtrlKeyReleased()
{

}

void FInputProcessor::LAltKeyPressed()
{
}

void FInputProcessor::LAltKeyReleased()
{
}

void FInputProcessor::LShiftKeyPressed()
{

}

void FInputProcessor::LShiftKeyReleased()
{

}

void FInputProcessor::SpaceKeyPressed()
{

}

void FInputProcessor::SpaceKeyReleased()
{

}

void FInputProcessor::MouseLeftReleased()
{

}

void FInputProcessor::MouseLeftPressed()
{

}

void FInputProcessor::MouseRightReleased()
{

}

void FInputProcessor::MouseRightPressed()
{

}

void FInputProcessor::PressedNumKey(int32 NumKey)
{

}

bool FInputProcessor::GetIsComplete() const
{
    return AsyncTaskNum == 0;
}

void FInputProcessor::SetPawn(FOwnerPawnType* NewPawnPtr)
{
	OnwerPawnPtr = NewPawnPtr;
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
	return bIsRequestQuit.load() || bIsExiting;
}

void FInputProcessor::MoveForward(const FInputActionValue& InputActionValue)
{
}

void FInputProcessor::MoveRight(const FInputActionValue& InputActionValue)
{
}

void FInputProcessor::MoveUp(const FInputActionValue& InputActionValue)
{
}

void FInputProcessor::MouseWheel(const FInputActionValue& InputActionValue)
{
}

void FInputProcessor::AddPitchInput(const FInputActionValue& InputActionValue)
{
}

void FInputProcessor::AddYawInput(const FInputActionValue& InputActionValue)
{
}

void FInputProcessor::F1KeyPressed()
{

}

void FInputProcessor::F2KeyPressed()
{

}

void FInputProcessor::LCtrlKeyPressed()
{

}

