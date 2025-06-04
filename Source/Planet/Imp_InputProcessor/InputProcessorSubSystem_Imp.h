// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <type_traits>

#include "InputProcessorSubSystemBase.h"

#include "HorseProcessor.h"
#include "HumanProcessor.h"

#include "InputProcessorSubSystem_Imp.generated.h"

/**
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UInputProcessorSubSystem_Imp : public UInputProcessorSubSystemBase
{
	GENERATED_BODY()

public:
	static UInputProcessorSubSystem_Imp* GetInstance();

	template <typename ProcessorType>
	void SwitchToProcessor();

	template <typename ProcessorType>
	void SwitchToProcessor(
		const FInitSwitchFunc<ProcessorType>& InitSwitchFunc,
		const FOnQuitFunc& OnQuitFunc = nullptr
		);

private:
};

template <typename ProcessorType>
void UInputProcessorSubSystem_Imp::SwitchToProcessor()
{
	SwitchToProcessor<ProcessorType>(nullptr);
}

template <typename ProcessorType>
void UInputProcessorSubSystem_Imp::SwitchToProcessor(
	const FInitSwitchFunc<ProcessorType>& InitSwitchFunc,
	const FOnQuitFunc& OnQuitFunc
	)
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
		ActionProcessSPtr->SetOnQuitFunc(OnQuitFunc);
	}
	else
	{
		auto PawnPtr = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		auto CharacterPtr = Cast<typename ProcessorType::FOwnerPawnType>(PawnPtr);
		ActionProcessSPtr = MakeShared<ProcessorType>(CharacterPtr);
		ActionProcessSPtr->SetOnQuitFunc(OnQuitFunc);
	}

	if (InitSwitchFunc)
	{
		InitSwitchFunc(ActionProcessSPtr.Get());
	}
	CurrentProcessorSPtr = ActionProcessSPtr;

	CurrentProcessorSPtr->EnterAction();
}
