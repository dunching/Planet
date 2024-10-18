// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "GAEvent_Helper.h"

#include "CharacterRisingTips.generated.h"

class ACharacterBase;
class UFightingTipsItem;

class UToolIcon;

/**
 * 浮动跳字
 */
UCLASS()
class PLANET_API UCharacterRisingTips : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	bool ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

protected:

	enum class EType
	{
		kBaseDamage,
		kTrueDamage,
		kGold,
		kWood,
		kWater,
		kFire,
		kSoil,
		kTreatment,
	};

	enum EIconIndex
	{
		kIcon_Treatment,
		kIcon_BaseDamage,
	};

	void ProcessGAEVentImp(EType Type, const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayMyAnimation(
		bool bIsCritical_In
	);

	UFUNCTION(BlueprintCallable)
	void PlayAnimationFinished();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FLinearColor TreatmentColor = FLinearColor(0.000000, 1.000000, 0.191129, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FLinearColor BaseDamageColor = FLinearColor(0.000000, 1.000000, 0.191129, 1.000000);

private:

	bool ResetPosition(float InDeltaTime);

	ACharacterBase* TargetCharacterPtr = nullptr;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	bool bIsCritical = false;
};

UCLASS()
class PLANET_API UCharacterRisingTips1 : public UCharacterRisingTips
{
	GENERATED_BODY()

public:

};
