
#include "InputProcessorSubSystem.h"

#include "EnhancedInputComponent.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "InputProcessor.h"
#include "ThreadSycAction.h"
#include "Pawn/InputComponent/ZYInputComponent.h"

TSharedPtr<FInputProcessor>& UInputProcessorSubSystem::GetCurrentAction()
{
	return CurrentProcessorSPtr;
}

void UInputProcessorSubSystem::BindAction(UZYInputComponent* InputComponentPtr, UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InputComponentPtr->MoveForwardActionPtr, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
		EnhancedInputComponent->BindAction(InputComponentPtr->MoveRightActionPtr, ETriggerEvent::Triggered, this, &ThisClass::MoveRight);

		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::MouseY], ETriggerEvent::Triggered, this, &ThisClass::AddPitchInput);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::MouseX], ETriggerEvent::Triggered, this, &ThisClass::AddYawInput);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::MouseWheelAxis], ETriggerEvent::Triggered, this, &ThisClass::MouseWheel);

		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::LeftMouseButton], ETriggerEvent::Started, this, &ThisClass::MouseLeftPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::LeftMouseButton], ETriggerEvent::Completed, this, &ThisClass::MouseLeftReleased);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::RightMouseButton], ETriggerEvent::Started, this, &ThisClass::MouseRightPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::RightMouseButton], ETriggerEvent::Completed, this, &ThisClass::MouseRightReleased);

		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Zero], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 0);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::One], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 1);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Two], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 2);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Three], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 3);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Four], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 4);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Five], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 5);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Six], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 6);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Seven], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 7);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Eight], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 8);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Nine], ETriggerEvent::Started, this, &ThisClass::PressedNumKeyIndex, 9);

		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::B], ETriggerEvent::Started, this, &ThisClass::BKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::E], ETriggerEvent::Started, this, &ThisClass::EKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::F], ETriggerEvent::Started, this, &ThisClass::FKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::G], ETriggerEvent::Started, this, &ThisClass::GKeyPressed);

		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::Escape], ETriggerEvent::Started, this, &ThisClass::ESCKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::LeftControl], ETriggerEvent::Started, this, &ThisClass::LCtrlKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::LeftControl], ETriggerEvent::Completed, this, &ThisClass::LCtrlKeyReleased);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::LeftShift], ETriggerEvent::Started, this, &ThisClass::LShiftKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::LeftShift], ETriggerEvent::Completed, this, &ThisClass::LShiftKeyReleased);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::SpaceBar], ETriggerEvent::Started, this, &ThisClass::SpaceKeyPressed);
		EnhancedInputComponent->BindAction(InputComponentPtr->KeyEventMap[EKeys::SpaceBar], ETriggerEvent::Completed, this, &ThisClass::SpaceKeyReleased);
	}
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

void UInputProcessorSubSystem::ESCKeyPressed()
{
	CurrentProcessorSPtr->ESCKeyPressed();
}

void UInputProcessorSubSystem::LCtrlKeyPressed()
{
	CurrentProcessorSPtr->LCtrlKeyPressed();
}

void UInputProcessorSubSystem::LCtrlKeyReleased()
{
	CurrentProcessorSPtr->LCtrlKeyReleased();
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

void UInputProcessorSubSystem::TickImp()
{
	decltype(ActionCacheSet)NeedRemoveSet;
	for (auto Iter : ActionCacheSet)
	{
		if (Iter.IsValid())
		{
			if (Iter->bHaveDone)
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
}

TSharedPtr<FInputProcessor> UInputProcessorSubSystem::SwitchActionProcessImp(
	EInputProcessorType ProcessType
)
{
	check(0);

	return nullptr;
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

void UInputProcessorSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UInputProcessorSubSystem::Deinitialize()
{
	if (CurrentProcessorSPtr)
	{
		CurrentProcessorSPtr->QuitAction();
	}

	Super::Deinitialize();
}

void UInputProcessorSubSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentInterval += DeltaTime;
	if (CurrentInterval > Interval)
	{
		CurrentInterval = 0.f;
		TickImp();
	}
}

TStatId UInputProcessorSubSystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UInputProcessorSubSystem, STATGROUP_Tickables);
}

UInputProcessorSubSystem* UInputProcessorSubSystem::GetInstance()
{
	return Cast<UInputProcessorSubSystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UInputProcessorSubSystem::StaticClass()));
}
