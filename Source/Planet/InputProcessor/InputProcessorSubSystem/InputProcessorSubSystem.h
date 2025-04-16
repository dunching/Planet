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

/**
 * 对于输入处理方式
 * 如正常模式下WASD和鼠标会控制玩家角色
 * 在浏览菜单时WASD则是其他的行为
 */
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

	template<typename ProcessorType>
	void SwitchToProcessor();

	template<typename ProcessorType>
	void SwitchToProcessor(
		const FInitSwitchFunc<ProcessorType>& InitSwitchFunc
	);

	void ResetProcessor();

	FDelegateHandle AddKeyEvent(FKey Key, const std::function<void(EInputEvent)>& KeyEvent);

	void RemoveKeyEvent(FDelegateHandle DelegateHandle);

	virtual bool InputKey(
		const FInputKeyEventArgs& EventArgs
	) ;

	virtual bool InputAxis(
		FViewport* Viewport,
		FInputDeviceId InputDevice,
		FKey Key,
		float Delta,
		float DeltaTime,
		int32 NumSamples = 1,
		bool bGamepad = false
	) ;
	
protected:

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