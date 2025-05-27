// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"
#include <AIController.h>

#include "GenerateTypes.h"
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
	virtual void NativeConstruct() override;

	/**
	 * 
	 * @param ReceivedEventModifyDataCallback 
	 * @return 是否要显示，如果数据需要过滤或者数据无效 则不显示
	 */
	bool SetData(
		const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
		);

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
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayDamageAnimation(
		bool bIsCritical_In,
		bool bIsEvade_In,
		EElementalType ElementalType
		);

	/**
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayTreatmentAnimation(
		);

	/**
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayStaminaAnimation();

	/**
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayManaAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayAnimationFinished();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FLinearColor TreatmentColor = FLinearColor(0.000000, 1.000000, 0.191129, 1.000000);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = SizeOption)
	FLinearColor BaseDamageColor = FLinearColor(0.000000, 1.000000, 0.191129, 1.000000);

private:
	void EndRising();

	ACharacterBase* TargetCharacterPtr = nullptr;
};
