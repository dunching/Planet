
#include "InputActions.h"

#include "EnhancedInputComponent.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

void UInputActions::InitialInputMapping()
{
	InputMappingContext = NewObject<UInputMappingContext>();

	{
		MoveForwardActionPtr = NewObject<UInputAction>();
		MoveForwardActionPtr->ValueType = EInputActionValueType::Axis1D;
		InputMappingContext->MapKey(MoveForwardActionPtr, EKeys::W);
		{
			auto& EnhancedActionKeyMapping = InputMappingContext->MapKey(MoveForwardActionPtr, EKeys::S);
			{
				auto InputModifierPtr = NewObject<UInputModifierNegate>();
				EnhancedActionKeyMapping.Modifiers.Add(InputModifierPtr);
			}
		}
	}
	{
		MoveRightActionPtr = NewObject<UInputAction>();
		MoveRightActionPtr->ValueType = EInputActionValueType::Axis1D;
		InputMappingContext->MapKey(MoveRightActionPtr, EKeys::D);
		{
			auto& EnhancedActionKeyMapping = InputMappingContext->MapKey(MoveRightActionPtr, EKeys::A);
			{
				auto InputModifierPtr = NewObject<UInputModifierNegate>();
				EnhancedActionKeyMapping.Modifiers.Add(InputModifierPtr);
			}
		}
	}
	{
		AddPitchActionPtr = NewObject<UInputAction>();
		AddPitchActionPtr->ValueType = EInputActionValueType::Axis1D;
		InputMappingContext->MapKey(AddPitchActionPtr, EKeys::MouseY);
	}
	{
		AddYawActionPtr = NewObject<UInputAction>();
		AddYawActionPtr->ValueType = EInputActionValueType::Axis1D;
		InputMappingContext->MapKey(AddYawActionPtr, EKeys::MouseX);
	}
}
