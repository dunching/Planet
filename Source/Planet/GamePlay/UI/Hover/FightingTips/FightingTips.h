// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "GAEvent_Helper.h"

#include "FightingTips.generated.h"

class ACharacterBase;
class UFightingTipsItem;

class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UFightingTips : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void ProcessGAEVent(const FGameplayAbilityTargetData_GAEvent& GAEvent);

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void PlayMyAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayAnimationFinished();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UFightingTipsItem>FightingTipsItemClass;

private:

	bool ResetPosition(float InDeltaTime);

	ACharacterBase* TargetCharacterPtr = nullptr;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	FVector2D SizeBox = FVector2D::ZeroVector;

};
