 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "HoverWidgetBase.h"


#include "CharacterRisingTips.generated.h"

struct FOnEffectedTawrgetCallback;

class ACharacterBase;
class UFightingTipsItem;

class UToolIcon;

/**
 * 浮动跳字
 */
UCLASS()
class PLANET_API UCharacterRisingTips : public UHoverWidgetBase
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	void SetData(const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback);
	
protected:

	virtual FVector GetHoverPosition() override;

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

	/**
	 * 
	 * @param bIsCritical_In	是否暴击
	 * @param bIsTreatment		是否是治疗
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayMyAnimation(
		bool bIsCritical_In,
		bool bIsTreatment,
		EElementalType ElementalType
	);

	UFUNCTION(BlueprintCallable)
	void PlayAnimationFinished();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FLinearColor TreatmentColor = FLinearColor(0.000000, 1.000000, 0.191129, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FLinearColor BaseDamageColor = FLinearColor(0.000000, 1.000000, 0.191129, 1.000000);

private:

	ACharacterBase* TargetCharacterPtr = nullptr;
};
