// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "GAEvent_Helper.h"

#include "FightingTipsItem.generated.h"

class ACharacterBase;

class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UFightingTipsItem : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

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

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void ProcessGAEVent(EType Type, const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FVector2D Size = FVector2D(40.f, 40.f);

private:

	FVector2D SizeBox = FVector2D::ZeroVector;

};
