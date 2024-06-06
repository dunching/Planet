// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "GAEvent_Helper.h"

#include "CharacterAttibutes.generated.h"

class FTalent_Base;

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
struct FElementPropertySet : public FBasePropertySet
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWuXingType WuXingType = EWuXingType::kFire;

protected:

};

USTRUCT(BlueprintType)
struct FElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FElementPropertySet GoldElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FElementPropertySet WoodElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FElementPropertySet WaterElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FElementPropertySet FireElement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FElementPropertySet SoilElement;
};

USTRUCT(BlueprintType)
struct PLANET_API FCharacterAttributes
{
	GENERATED_USTRUCT_BODY()

	FCharacterAttributes();

	virtual ~FCharacterAttributes();

	void ProcessGAEVent(const FGameplayAbilityTargetData_GAEvent& GAEvent);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name;
	
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
	FElement Element;

	FTalent_Base* TalentPtr = nullptr;
#pragma endregion

	// 基础属性
#pragma region 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet BaseAttackPower;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Penetration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet PercentPenetration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Resistance;

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

struct PLANET_API FScopeCharacterAttributes
{
	FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes);
};

#pragma endregion CharacterAttributes
