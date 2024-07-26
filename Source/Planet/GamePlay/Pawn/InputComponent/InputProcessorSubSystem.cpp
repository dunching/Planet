
#include "InputProcessorSubSystem.h"

#include "EnhancedInputComponent.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include <GameDelegates.h>

#include "LogWriter.h"

#include "InputProcessor.h"
#include "Pawn/InputComponent/InputActions.h"

void UInputProcessorSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    InputActionsPtr = NewObject<UInputActions>();
    InputActionsPtr->InitialInputMapping();

	CurrentProcessorSPtr = MakeShared<FInputProcessor>(nullptr);

	TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::Tick), Frequency);

#if WITH_EDITOR
	FGameDelegates::Get().GetExitCommandDelegate().AddLambda([]() {
		PRINTINVOKEINFO();
		});
	FGameDelegates::Get().GetEndPlayMapDelegate().AddLambda([]() {
		PRINTINVOKEINFO();
		});
	FCoreDelegates::OnPreExit.AddLambda([]() {
		PRINTINVOKEINFO();
		});
	FCoreDelegates::OnEnginePreExit.AddLambda([]() {
		PRINTINVOKEINFO();
		});
	FWorldDelegates::OnWorldBeginTearDown.AddLambda([](UWorld*) {
		PRINTINVOKEINFO();
		});
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
    return Cast<UInputProcessorSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UInputProcessorSubSystem::StaticClass()));
}

TSharedPtr<FInputProcessor>& UInputProcessorSubSystem::GetCurrentAction()
{
	return CurrentProcessorSPtr;
}

void UInputProcessorSubSystem::BindAction(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InputActionsPtr->MoveForwardActionPtr, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
		EnhancedInputComponent->BindAction(InputActionsPtr->MoveRightActionPtr, ETriggerEvent::Triggered, this, &ThisClass::MoveRight);

		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::MouseY], ETriggerEvent::Triggered, this, &ThisClass::AddPitchInput);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::MouseX], ETriggerEvent::Triggered, this, &ThisClass::AddYawInput);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::MouseWheelAxis], ETriggerEvent::Triggered, this, &ThisClass::MouseWheel);

		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftMouseButton], ETriggerEvent::Started, this, &ThisClass::MouseLeftPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftMouseButton], ETriggerEvent::Completed, this, &ThisClass::MouseLeftReleased);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::RightMouseButton], ETriggerEvent::Started, this, &ThisClass::MouseRightPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::RightMouseButton], ETriggerEvent::Completed, this, &ThisClass::MouseRightReleased);

		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Zero], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 0);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::One], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 1);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Two], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 2);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Three], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 3);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Four], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 4);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Five], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 5);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Six], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 6);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Seven], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 7);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Eight], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 8);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Nine], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 9);

		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::F1], ETriggerEvent::Started, this, &ThisClass::F1KeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::F2], ETriggerEvent::Started, this, &ThisClass::F2KeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::F10], ETriggerEvent::Started, this, &ThisClass::F10KeyPressed);

		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Q], ETriggerEvent::Started, this, &ThisClass::QKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::W], ETriggerEvent::Started, this, &ThisClass::WKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::E], ETriggerEvent::Started, this, &ThisClass::EKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::E], ETriggerEvent::Completed, this, &ThisClass::EKeyReleased);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::R], ETriggerEvent::Started, this, &ThisClass::RKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::F], ETriggerEvent::Started, this, &ThisClass::FKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::A], ETriggerEvent::Started, this, &ThisClass::AKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::S], ETriggerEvent::Started, this, &ThisClass::SKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::D], ETriggerEvent::Started, this, &ThisClass::DKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::G], ETriggerEvent::Started, this, &ThisClass::GKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::H], ETriggerEvent::Started, this, &ThisClass::HKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::X], ETriggerEvent::Started, this, &ThisClass::XKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::C], ETriggerEvent::Started, this, &ThisClass::CKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::V], ETriggerEvent::Started, this, &ThisClass::VKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::B], ETriggerEvent::Started, this, &ThisClass::BKeyPressed);

		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Tab], ETriggerEvent::Started, this, &ThisClass::TabKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::Escape], ETriggerEvent::Started, this, &ThisClass::ESCKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftControl], ETriggerEvent::Started, this, &ThisClass::LCtrlKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftControl], ETriggerEvent::Completed, this, &ThisClass::LCtrlKeyReleased);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftAlt], ETriggerEvent::Started, this, &ThisClass::LAltKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftAlt], ETriggerEvent::Completed, this, &ThisClass::LAltKeyReleased);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftShift], ETriggerEvent::Started, this, &ThisClass::LShiftKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::LeftShift], ETriggerEvent::Completed, this, &ThisClass::LShiftKeyReleased);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::SpaceBar], ETriggerEvent::Started, this, &ThisClass::SpaceKeyPressed);
		EnhancedInputComponent->BindAction(InputActionsPtr->KeyEventMap[EKeys::SpaceBar], ETriggerEvent::Completed, this, &ThisClass::SpaceKeyReleased);
	}
}

void UInputProcessorSubSystem::InputKey(const FInputKeyParams& Params)
{
	CurrentProcessorSPtr->InputKey(Params);
}

void UInputProcessorSubSystem::MoveForward(const FInputActionValue& InputActionValue)
{
	CurrentProcessorSPtr->MoveForward(InputActionValue);
}

void UInputProcessorSubSystem::MoveRight(const FInputActionValue& InputActionValue)
{
	CurrentProcessorSPtr->MoveRight(InputActionValue);
}

void UInputProcessorSubSystem::MoveUp(const FInputActionValue& InputActionValue)
{
	CurrentProcessorSPtr->MoveUp(InputActionValue);
}

void UInputProcessorSubSystem::MouseWheel(const FInputActionValue& InputActionValue)
{
	CurrentProcessorSPtr->MouseWheel(InputActionValue);
}

void UInputProcessorSubSystem::AddPitchInput(const FInputActionValue& InputActionValue)
{
	CurrentProcessorSPtr->AddPitchInput(InputActionValue);
}

void UInputProcessorSubSystem::AddYawInput(const FInputActionValue& InputActionValue)
{
	CurrentProcessorSPtr->AddYawInput(InputActionValue);
}

void UInputProcessorSubSystem::F1KeyPressed()
{
	CurrentProcessorSPtr->F1KeyPressed();
}

void UInputProcessorSubSystem::F2KeyPressed()
{
	CurrentProcessorSPtr->F2KeyPressed();
}

void UInputProcessorSubSystem::F10KeyPressed()
{
	CurrentProcessorSPtr->F10KeyPressed();
}

void UInputProcessorSubSystem::WKeyPressed()
{
	CurrentProcessorSPtr->WKeyPressed();
}

void UInputProcessorSubSystem::WKeyReleased()
{
	CurrentProcessorSPtr->WKeyReleased();
}

void UInputProcessorSubSystem::AKeyPressed()
{
	CurrentProcessorSPtr->AKeyPressed();
}

void UInputProcessorSubSystem::AKeyReleased()
{
	CurrentProcessorSPtr->AKeyReleased();
}

void UInputProcessorSubSystem::SKeyPressed()
{
	CurrentProcessorSPtr->SKeyPressed();
}

void UInputProcessorSubSystem::SKeyReleased()
{
	CurrentProcessorSPtr->SKeyReleased();
}

void UInputProcessorSubSystem::DKeyPressed()
{
	CurrentProcessorSPtr->DKeyPressed();
}

void UInputProcessorSubSystem::DKeyReleased()
{
	CurrentProcessorSPtr->DKeyReleased();
}

void UInputProcessorSubSystem::QKeyPressed()
{
	CurrentProcessorSPtr->QKeyPressed();
}

void UInputProcessorSubSystem::EKeyPressed()
{
	CurrentProcessorSPtr->EKeyPressed();
}

void UInputProcessorSubSystem::EKeyReleased()
{
	CurrentProcessorSPtr->EKeyReleased();
}

void UInputProcessorSubSystem::RKeyPressed()
{
	CurrentProcessorSPtr->RKeyPressed();
}

void UInputProcessorSubSystem::TKeyPressed()
{
	CurrentProcessorSPtr->TKeyPressed();
}

void UInputProcessorSubSystem::YKeyPressed()
{
	CurrentProcessorSPtr->YKeyPressed();
}

void UInputProcessorSubSystem::FKeyPressed()
{
	CurrentProcessorSPtr->FKeyPressed();
}

void UInputProcessorSubSystem::CKeyPressed()
{
	CurrentProcessorSPtr->CKeyPressed();
}

void UInputProcessorSubSystem::XKeyPressed()
{
	CurrentProcessorSPtr->XKeyPressed();
}

void UInputProcessorSubSystem::VKeyPressed()
{
	CurrentProcessorSPtr->VKeyPressed();
}

void UInputProcessorSubSystem::BKeyPressed()
{
	CurrentProcessorSPtr->BKeyPressed();
}

void UInputProcessorSubSystem::GKeyPressed()
{
	CurrentProcessorSPtr->GKeyPressed();
}

void UInputProcessorSubSystem::HKeyPressed()
{
	CurrentProcessorSPtr->HKeyPressed();
}

void UInputProcessorSubSystem::ESCKeyPressed()
{
	CurrentProcessorSPtr->ESCKeyPressed();
}

void UInputProcessorSubSystem::TabKeyPressed()
{
	CurrentProcessorSPtr->TabKeyPressed();
}

void UInputProcessorSubSystem::LCtrlKeyPressed()
{
	CurrentProcessorSPtr->LCtrlKeyPressed();
}

void UInputProcessorSubSystem::LCtrlKeyReleased()
{
	CurrentProcessorSPtr->LCtrlKeyReleased();
}

void UInputProcessorSubSystem::LAltKeyPressed()
{
	CurrentProcessorSPtr->LAltKeyPressed();
}

void UInputProcessorSubSystem::LAltKeyReleased()
{
	CurrentProcessorSPtr->LAltKeyReleased();
}

void UInputProcessorSubSystem::LShiftKeyPressed()
{
	CurrentProcessorSPtr->LShiftKeyPressed();
}

void UInputProcessorSubSystem::LShiftKeyReleased()
{
	CurrentProcessorSPtr->LShiftKeyReleased();
}

void UInputProcessorSubSystem::SpaceKeyPressed()
{
	CurrentProcessorSPtr->SpaceKeyPressed();
}

void UInputProcessorSubSystem::SpaceKeyReleased()
{
	CurrentProcessorSPtr->SpaceKeyReleased();
}

void UInputProcessorSubSystem::MouseLeftReleased()
{
	CurrentProcessorSPtr->MouseLeftReleased();

	if (OnKeyPressedMap.Contains(EKeys::LeftMouseButton))
	{
		OnKeyPressedMap[EKeys::LeftMouseButton].Broadcast(EInputEvent::IE_Released);
	}
}

void UInputProcessorSubSystem::MouseLeftPressed()
{
	CurrentProcessorSPtr->MouseLeftPressed();

	if (OnKeyPressedMap.Contains(EKeys::LeftMouseButton))
	{
		OnKeyPressedMap[EKeys::LeftMouseButton].Broadcast(EInputEvent::IE_Pressed);
	}
}

void UInputProcessorSubSystem::MouseRightReleased()
{
	CurrentProcessorSPtr->MouseRightReleased();
}

void UInputProcessorSubSystem::MouseRightPressed()
{
	CurrentProcessorSPtr->MouseRightPressed();
}

void UInputProcessorSubSystem::PressedNumKeyIndex(int32 KeyVal)
{
	CurrentProcessorSPtr->PressedNumKey(KeyVal);
}

bool UInputProcessorSubSystem::Tick(float DeltaTime)
{
	decltype(ActionCacheSet)NeedRemoveSet;
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

TSharedPtr<FInputProcessor> UInputProcessorSubSystem::SwitchActionProcessImp(
	EInputProcessorType ProcessType
)
{
	check(0);

	return nullptr;
}

void UInputProcessorSubSystem::ResetProcessor()
{
	if (CurrentProcessorSPtr)
	{
		CurrentProcessorSPtr->QuitAction();
	}
}

FDelegateHandle UInputProcessorSubSystem::AddKeyEvent(FKey Key, const std::function<void(EInputEvent)>& KeyEvent)
 {
 	if (OnKeyPressedMap.Contains(Key))
 	{
 		return OnKeyPressedMap[Key].AddLambda(KeyEvent);
 	}
  	else
  	{
  		FSYKeyEvent SYKeyEvent; 
		auto Delegate = SYKeyEvent.AddLambda(KeyEvent);
  		OnKeyPressedMap.Add(Key,  SYKeyEvent );
  		return Delegate;
  	}
 }

void UInputProcessorSubSystem::RemoveKeyEvent(FDelegateHandle DelegateHandle)
{
	for (auto & Iter : OnKeyPressedMap)
	{
		Iter.Value.Remove(DelegateHandle);
	}
}
