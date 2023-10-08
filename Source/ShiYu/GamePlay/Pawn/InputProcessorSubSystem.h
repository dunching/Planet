// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <type_traits>

#include "CoreMinimal.h"

#include "InputActionValue.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GenerateType.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"

#include "InputProcessorSubSystem.generated.h"

class UInputComponent;

class UZYInputComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FSYKeyEvent, EInputEvent);

UCLASS(BlueprintType, Blueprintable)
class UInputProcessorSubSystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	template<typename ActionProcessType>
	using FInitSwitchFunc = std::function<void(ActionProcessType*)>;

	static UInputProcessorSubSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	TSharedPtr<FInputProcessor>& GetCurrentAction();

	void BindAction(UZYInputComponent* InputComponentPtr, UInputComponent* PlayerInputComponent);

	template<typename ActionProcessType>
	void SwitchActionProcess();

	template<typename ActionProcessType>
	void SwitchActionProcess(
		const FInitSwitchFunc<ActionProcessType>& InitSwitchFunc
	);

	FDelegateHandle AddKeyEvent(FKey Key, const std::function<void(EInputEvent)>& KeyEvent);

	void RemoveKeyEvent(FDelegateHandle DelegateHandle);

protected:

	void MoveForward(const FInputActionValue& InputActionValue);

	void MoveRight(const FInputActionValue& InputActionValue);

	void MoveUp(const FInputActionValue& InputActionValue);

	void MouseWheel(const FInputActionValue& InputActionValue);

	void AddPitchInput(const FInputActionValue& InputActionValue);

	void AddYawInput(const FInputActionValue& InputActionValue);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void MouseWheel(float Value);

	void AddPitchInput(float Value);

	void AddYawInput(float Value);

	void QKeyPressed();

	void WKeyPressed();

	void WKeyReleased();

	void AKeyPressed();

	void AKeyReleased();

	void SKeyPressed();

	void SKeyReleased();

	void DKeyPressed();

	void DKeyReleased();

	void EKeyPressed();

	void EKeyReleased();

	void RKeyPressed();

	void TKeyPressed();

	void YKeyPressed();

	void FKeyPressed();

	void CKeyPressed();

	void VKeyPressed();

	void BKeyPressed();

	void GKeyPressed();

	void ESCKeyPressed();

	void LCtrlKeyPressed();

	void LCtrlKeyReleased();

	void LShiftKeyPressed();

	void LShiftKeyReleased();

	void SpaceKeyPressed();

	void SpaceKeyReleased();

	void MouseLeftReleased();

	void MouseLeftPressed();

	void MouseRightReleased();

	void MouseRightPressed();

	void PressedNumKeyIndex(int32 KeyVal);

private:

	void TickImp();

	virtual TSharedPtr<FInputProcessor> SwitchActionProcessImp(
		EInputProcessorType ProcessType
	);

	TSet<TSharedPtr<FInputProcessor>> ActionCacheSet;

	TSharedPtr<FInputProcessor>CurrentProcessorSPtr;

	EInputProcessorType CurrentProcessType = EInputProcessorType::kNone;

	TMap<FKey, FSYKeyEvent> OnKeyPressedMap;

	float Interval = 1.f;

	float CurrentInterval = 0.f;

};

template<typename ActionProcessType>
void UInputProcessorSubSystem::SwitchActionProcess()
{
	SwitchActionProcess<ActionProcessType>(nullptr);
}

template<typename ActionProcessType>
void UInputProcessorSubSystem::SwitchActionProcess(const FInitSwitchFunc<ActionProcessType>& InitSwitchFunc)
{
	if constexpr (
		(std::is_same_v<ActionProcessType, FInputProcessor>)||
		(std::is_same_v<ActionProcessType, HorseProcessor::FHorseProcessor>)||
		(std::is_same_v<ActionProcessType, HumanProcessor::FHumanProcessor>)
		)
	{
		return;
	}

	if (
		(ActionProcessType::InputProcessprType == EInputProcessorType::kNone)
		)
	{
		return;
	}

	if (
		(ActionProcessType::InputProcessprType == CurrentProcessType)
		)
	{
		if (InitSwitchFunc)
		{
			auto ActionProcessPtr = dynamic_cast<ActionProcessType*>(CurrentProcessorSPtr.Get());
			if (ActionProcessPtr)
			{
				InitSwitchFunc(ActionProcessPtr);
			}
		}
		CurrentProcessorSPtr->ReEnterAction();
		return;
	}

	CurrentProcessType = ActionProcessType::InputProcessprType;

	if (CurrentProcessorSPtr.IsValid())
	{
		CurrentProcessorSPtr->QuitAction();
	}

	ActionCacheSet.Add(CurrentProcessorSPtr);

	TSharedPtr<ActionProcessType> ActionProcessSPtr;

    if constexpr (std::is_base_of_v<HumanProcessor::FHumanProcessor, ActionProcessType>)
    {
		auto PawnPtr = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        auto CharacterPtr = Cast<typename ActionProcessType::FOwnerPawnType>(PawnPtr);
        ActionProcessSPtr = MakeShared<ActionProcessType>(CharacterPtr);
    }
 	else if constexpr (std::is_base_of_v<HorseProcessor::FHorseProcessor, ActionProcessType>)
 	{
		auto PawnPtr = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		auto CharacterPtr = Cast<typename ActionProcessType::FOwnerPawnType>(PawnPtr);
		ActionProcessSPtr = MakeShared<ActionProcessType>(CharacterPtr);
 	}
 	else
 	{
 		ensure(0);
 		ActionProcessSPtr = MakeShared<ActionProcessType>();
 	}

	if (InitSwitchFunc)
	{
		InitSwitchFunc(ActionProcessSPtr.Get());
	}
	CurrentProcessorSPtr = ActionProcessSPtr;

	CurrentProcessorSPtr->EnterAction();
}