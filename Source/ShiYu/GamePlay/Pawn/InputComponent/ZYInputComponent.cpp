
#include "ZYInputComponent.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

UZYInputComponent::UZYInputComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}
void UZYInputComponent::InitialInputMapping()
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
		EKeys::LeftMouseButton ,
		EKeys::RightMouseButton ,
		EKeys::Zero ,
		EKeys::One ,
		EKeys::Two ,
		EKeys::Three ,
		EKeys::Four ,
		EKeys::Five ,
		EKeys::Six ,
		EKeys::Seven ,
		EKeys::Eight ,
		EKeys::Nine ,
		EKeys::B ,
		EKeys::E ,
		EKeys::F ,
		EKeys::G ,
		EKeys::Escape ,
		EKeys::LeftControl ,
		EKeys::LeftShift ,
		EKeys::SpaceBar ,
	};
	for (const auto& Iter : BoolEventAry)
	{
		auto InputActionPtr = NewObject<UInputAction>();

		KeyEventMap.Add(Iter, InputActionPtr);

		InputActionPtr->ValueType = EInputActionValueType::Boolean;
		InputMappingContext->MapKey(InputActionPtr, Iter);
	}
}

void UZYInputComponent::BeginPlay()
{
	Super::BeginPlay();
}

FName UZYInputComponent::ComponentName = TEXT("ZYInputComponent");

