// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenerateType.h"

#include "CharacterAttibutes.generated.h"

#pragma region CharacterAttributes
USTRUCT(BlueprintType)
struct FBaseProperty
{
	GENERATED_USTRUCT_BODY()

public:

	FBaseProperty();

	int32 GetCurrentValue() const;

	void SetCurrentValue(int32 val);

	void AddCurrentValue(int32 val);

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentValue = 100;

};

USTRUCT(BlueprintType)
struct FBasePropertySet
{
	GENERATED_USTRUCT_BODY()

public:

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;
	
	FBaseProperty & GetCurrentProperty();

	int32 GetMaxValue()const;

	FBaseProperty& GetMaxProperty();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty CurrentValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty MinValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty MaxValue;

};

USTRUCT(BlueprintType)
struct FElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet GoldElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet WoodElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet WaterElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet FireElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet SoilElement;
};

USTRUCT(BlueprintType)
struct PLANET_API FCharacterAttributes
{
	GENERATED_USTRUCT_BODY()

	virtual ~FCharacterAttributes();

	void ProcessGAEVent(const FGameplayAbilityTargetData_GAEvent& GAEvent);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD_Penetration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD_PercentPenetration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD_Resistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP_Penetration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP_PercentPenetration;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP_Resistance;

	// 基础 属性：力道、根骨、身法、洞察、天资
#pragma region 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet LiDao;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet GenGu;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet ShenFa;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet DongCha;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet TianZi;
#pragma endregion
	
	// 天赋属性
#pragma region 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet NuQi;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FElement Element;
#pragma endregion

	// 基础属性
#pragma region 
	// 攻击速度、技能释放速度
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet GAPerformSpeed;

	// 生命值
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet HP;

	// 生命值回复速率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet HPReplay;

	// 体力
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet PP;

	// 体力回复速率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet PPReplay;

	// 法力值
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet ManaValue;

	// 法力值回复速率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet ManaValueReplay;

	// 闪避几率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Evade;

	// 命中率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet HitRate;

	// 韧性
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Toughness;

	// 会心率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet CriticalHitRate;

	// 会心伤害
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet CriticalDamage;
#pragma endregion

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet WalkingSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet RunningSpeedOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet RunningConsume;

};
#pragma endregion CharacterAttributes