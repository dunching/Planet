
#include "InputActions.h"

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

	TArray<FKey> Axis1DEventAry
	{
		EKeys::MouseY ,
		EKeys::MouseX ,
		EKeys::MouseWheelAxis ,
	};
	for (const auto& Iter : Axis1DEventAry)
	{
		auto InputActionPtr = NewObject<UInputAction>();

		KeyEventMap.Add(Iter, InputActionPtr);

		InputActionPtr->ValueType = EInputActionValueType::Axis1D;
		InputMappingContext->MapKey(InputActionPtr, Iter);
	}

	TArray<FKey> BoolEventAry
	{
		EKeys::LeftMouseButton,
		EKeys::RightMouseButton,

		EKeys::Zero,
		EKeys::One,
		EKeys::Two,
		EKeys::Three,
		EKeys::Four,
		EKeys::Five,
		EKeys::Six,
		EKeys::Seven,
		EKeys::Eight,
		EKeys::Nine,

		EKeys::Q,
		EKeys::W,
		EKeys::E,
		EKeys::R,
		EKeys::T,
		EKeys::Y,
		EKeys::U,
		EKeys::I,
		EKeys::O,
		EKeys::P,
		EKeys::A,
		EKeys::S,
		EKeys::D,
		EKeys::F,
		EKeys::G,
		EKeys::H,
		EKeys::J,
		EKeys::K,
		EKeys::L,
		EKeys::Z,
		EKeys::X,
		EKeys::C,
		EKeys::V,
		EKeys::B,
		EKeys::N,
		EKeys::M,

		EKeys::Tab,
		EKeys::Escape,
		EKeys::LeftControl,
		EKeys::LeftAlt,
		EKeys::LeftShift,
		EKeys::SpaceBar,
	};
	for (const auto& Iter : BoolEventAry)
	{
		auto InputActionPtr = NewObject<UInputAction>();

		KeyEventMap.Add(Iter, InputActionPtr);

		InputActionPtr->ValueType = EInputActionValueType::Boolean;
		InputMappingContext->MapKey(InputActionPtr, Iter);
	}
}
