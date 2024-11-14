// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "UIInterfaces.h"
#include "GenerateType.h"

#include "ItemProxy.h"

#include "ActionConsumablesIcon.generated.h"

struct FStreamableHandle;

struct FSkillProxy;

/**
 *
 */
UCLASS()
class PLANET_API UActionConsumablesIcon :
	public UMyUserWidget,
	public IUnitIconInterface
{
	GENERATED_BODY()

public:

	using FOnValueChangedDelegateHandle = 
		TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	UActionConsumablesIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void UpdateState();

	TSharedPtr < FConsumableProxy> UnitPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

protected:

	void SetLevel();

	void SetRemainingCooldown(
		bool bCooldownIsReady,
		float RemainingTime,
		float Percent
	);

	void SetCanRelease(
		bool bIsReady_In
	);

	void SetItemType();

	void SetNum(int32 NewNum);

	void SetInputRemainPercent(bool bIsAcceptInput, float Percent);

	void SetDurationPercent(bool bIsHaveDuration, float Percent);

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySkillIsReady();

private:

	bool bIsReady_Previous = false;

	FOnValueChangedDelegateHandle OnValueChangedDelegateHandle;

};