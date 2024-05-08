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

class UInputActions;
class APlanetPlayerController;

DECLARE_MULTICAST_DELEGATE_OneParam(FSYKeyEvent, EInputEvent);

UCLASS(BlueprintType, Blueprintable)
class UInputProcessorSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	friend APlanetPlayerController;

	template<typename ProcessorType>
	using FInitSwitchFunc = std::function<void(ProcessorType*)>;

	static UInputProcessorSubSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	TSharedPtr<FInputProcessor>& GetCurrentAction();

	void BindAction(UInputComponent* PlayerInputComponent);

	template<typename ProcessorType>
	void SwitchToProcessor();

	template<typename ProcessorType>
	void SwitchToProcessor(
		const FInitSwitchFunc<ProcessorType>& InitSwitchFunc
	);

	FDelegateHandle AddKeyEvent(FKey Key, const std::function<void(EInputEvent)>& KeyEvent);

	void RemoveKeyEvent(FDelegateHandle DelegateHandle);

	UPROPERTY()
	UInputActions* InputActionsPtr = nullptr;

protected:

	void InputKey(const FInputKeyParams& Params);

	void MoveForward(const FInputActionValue& InputActionValue);

	void MoveRight(const FInputActionValue& InputActionValue);

	void MoveUp(const FInputActionValue& InputActionValue);

	void MouseWheel(const FInputActionValue& InputActionValue);

	void AddPitchInput(const FInputActionValue& InputActionValue);

	void AddYawInput(const FInputActionValue& InputActionValue);

	void QKeyPressed();

	void WKeyPressed();

	void WKeyReleased();

	void EKeyPressed();

	void EKeyReleased();

	void RKeyPressed();

	void TKeyPressed();

	void YKeyPressed();

	void AKeyPressed();

	void AKeyReleased();

	void SKeyPressed();

	void SKeyReleased();

	void DKeyPressed();

	void DKeyReleased();

	void FKeyPressed();

	void CKeyPressed();

	void VKeyPressed();

	void BKeyPressed();

	void GKeyPressed();

	void HKeyPressed();

	void ESCKeyPressed();

	void LCtrlKeyPressed();

	void LCtrlKeyReleased();

	void LAltKeyPressed();

	void LAltKeyReleased();

	void LShiftKeyPressed();

	void LShiftKeyReleased();

	void SpaceKeyPressed();

	void SpaceKeyReleased();

	void MouseLeftPressed();

	void MouseLeftReleased();

	void MouseRightPressed();

	void MouseRightReleased();

	void PressedNumKeyIndex(int32 KeyVal);

	UFUNCTION()
	bool Tick(float DeltaTime);

private:

	virtual TSharedPtr<FInputProcessor> SwitchActionProcessImp(
		EInputProcessorType ProcessType
	);

	TSet<TSharedPtr<FInputProcessor>> ActionCacheSet;

	TSharedPtr<FInputProcessor>CurrentProcessorSPtr;

	TMap<FKey, FSYKeyEvent> OnKeyPressedMap;

	const float Frequency = 1.f;

	bool bIsRunning = true;

	FTSTicker::FDelegateHandle TickDelegateHandle;
};

template<typename ProcessorType>
void UInputProcessorSubSystem::SwitchToProcessor()
{
	SwitchToProcessor<ProcessorType>(nullptr);
}

template<typename ProcessorType>
void UInputProcessorSubSystem::SwitchToProcessor(const FInitSwitchFunc<ProcessorType>& InitSwitchFunc)
{
	if constexpr (
		(std::is_same_v<ProcessorType, FInputProcessor>) ||
		(std::is_same_v<ProcessorType, HorseProcessor::FHorseProcessor>) ||
		(std::is_same_v<ProcessorType, HumanProcessor::FHumanProcessor>)
		)
	{
		return;
	}

	if (CurrentProcessorSPtr.IsValid())
	{
		CurrentProcessorSPtr->QuitAction();
	}

	ActionCacheSet.Add(CurrentProcessorSPtr);

	TSharedPtr<ProcessorType> ActionProcessSPtr;

	if constexpr (std::is_base_of_v<HumanProcessor::FHumanProcessor, ProcessorType>)
	{
		auto PawnPtr = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		auto CharacterPtr = Cast<typename ProcessorType::FOwnerPawnType>(PawnPtr);
		ActionProcessSPtr = MakeShared<ProcessorType>(CharacterPtr);
	}
	else
	{
		auto PawnPtr = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		auto CharacterPtr = Cast<typename ProcessorType::FOwnerPawnType>(PawnPtr);
		ActionProcessSPtr = MakeShared<ProcessorType>(CharacterPtr);
	}

	if (InitSwitchFunc)
	{
		InitSwitchFunc(ActionProcessSPtr.Get());
	}
	CurrentProcessorSPtr = ActionProcessSPtr;

	CurrentProcessorSPtr->EnterAction();
}