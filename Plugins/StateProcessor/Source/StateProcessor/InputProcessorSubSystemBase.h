// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <type_traits>

#include "CoreMinimal.h"
#include "InputProcessor.h"

#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"

#include "InputProcessorSubSystemBase.generated.h"

class UInputComponent;

class UInputActions;
class APlanetPlayerController;

DECLARE_MULTICAST_DELEGATE_OneParam(FSYKeyEvent, EInputEvent);

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGetInputProcessorSubSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class STATEPROCESSOR_API IGetInputProcessorSubSystemInterface
{
	GENERATED_BODY()

public:
	virtual UInputProcessorSubSystemBase* GetInputProcessorSubSystem()const = 0;
};

/**
 * 对于输入处理方式
 * 如正常模式下WASD和鼠标会控制玩家角色
 * 在浏览菜单时WASD则是其他的行为
 */
UCLASS(BlueprintType, Blueprintable)
class STATEPROCESSOR_API UInputProcessorSubSystemBase : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	friend APlanetPlayerController;

	template <typename ProcessorType>
	using FInitSwitchFunc = std::function<void(
		ProcessorType*
		)>;

	using FOnQuitFunc = std::function<void()>;

	static UInputProcessorSubSystemBase* GetInstanceBase();

	virtual void Initialize(
		FSubsystemCollectionBase& Collection
		) override;

	virtual void Deinitialize() override;

	TSharedPtr<FInputProcessor>& GetCurrentAction();

	void ResetProcessor();

	FDelegateHandle AddKeyEvent(
		FKey Key,
		const std::function<void(
			EInputEvent
			)>& KeyEvent
		);

	void RemoveKeyEvent(
		FDelegateHandle DelegateHandle
		);

	virtual bool InputKey(
		const FInputKeyEventArgs& EventArgs
		);

	virtual bool InputAxis(
	const FInputKeyEventArgs& EventArgs
		);

	void SwitchShowCursor(bool bIsShowCursor);

protected:
	template <typename ProcessorType>
	void SwitchToProcessorBase();

	template <typename ProcessorType>
	void SwitchToProcessorBase(
		const FInitSwitchFunc<ProcessorType>& InitSwitchFunc,
		const FOnQuitFunc& OnQuitFunc = nullptr
		);

	UFUNCTION()
	bool Tick(
		float DeltaTime
		);

	TSet<TSharedPtr<FInputProcessor>> ActionCacheSet;

	TSharedPtr<FInputProcessor> CurrentProcessorSPtr;

	TMap<FKey, FSYKeyEvent> OnKeyPressedMap;

private:
	const float Frequency = 1.f;

	bool bIsRunning = true;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};

template <typename ProcessorType>
void UInputProcessorSubSystemBase::SwitchToProcessorBase()
{
	SwitchToProcessorBase<ProcessorType>(nullptr);
}

template <typename ProcessorType>
void UInputProcessorSubSystemBase::SwitchToProcessorBase(
	const FInitSwitchFunc<ProcessorType>& InitSwitchFunc,
		const FOnQuitFunc& OnQuitFunc
	)
{
	if constexpr (
		(std::is_same_v<ProcessorType, FInputProcessor>) 
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

	auto PawnPtr = GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetPawn();
	auto CharacterPtr = Cast<typename ProcessorType::FOwnerPawnType>(PawnPtr);
	ActionProcessSPtr = MakeShared<ProcessorType>(CharacterPtr);
	ActionProcessSPtr->OnQuitFunc = OnQuitFunc;

	if (InitSwitchFunc)
	{
		InitSwitchFunc(ActionProcessSPtr.Get());
	}
	CurrentProcessorSPtr = ActionProcessSPtr;

	CurrentProcessorSPtr->EnterAction();
}
