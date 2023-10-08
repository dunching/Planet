
#include "InputProcessor.h"

#include "Animation/AnimInstanceBase.h"

FInputProcessor::FInputProcessor(APawn* CharacterPtr) :
	OnwerPawnPtr(CharacterPtr)
{

}

FInputProcessor::~FInputProcessor()
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

void FInputProcessor::CKeyPressed()
{

}

void FInputProcessor::VKeyPressed()
{

}

void FInputProcessor::GKeyPressed()
{
}

void FInputProcessor::LCtrlKeyReleased()
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

void FInputProcessor::MarkHaveDone()
{
	bHaveDone = true;
}

void FInputProcessor::ESCKeyPressed()
{
}

void FInputProcessor::EnterAction()
{
	bNeedQuit = false;
}

void FInputProcessor::ReEnterAction()
{

}

void FInputProcessor::QuitAction()
{
	bNeedQuit = true;
}

void FInputProcessor::BeginDestroy()
{

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

void FInputProcessor::LCtrlKeyPressed()
{

}

