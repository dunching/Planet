// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "BaseData.h"
#include "GAEvent_Helper.h"

#include "CharacterAttibutes.generated.h"

class FTalent_Base;

#pragma region CharacterAttributes

struct FCharacterAttributes;

USTRUCT(BlueprintType)
struct PLANET_API FBasePropertySet 
{
	GENERATED_USTRUCT_BODY()

public:

	void AddCurrentValue(int32 NewValue, FGuid GUID);

	void RemoveCurrentValue(FGuid GUID);

	void SetCurrentValue(int32 NewValue, FGuid GUID);

	int32 GetCurrentValue()const;
	
	const FBaseProperty & GetCurrentProperty()const;

	int32 GetMaxValue()const;

	const FBaseProperty& GetMaxProperty()const;

	const FBaseProperty& GetMinProperty()const;

	template<typename Type>
	auto AddOnValueChanged(const Type& Func)
	{
		return CurrentValue.CallbackContainerHelper.AddOnValueChanged(Func);
	}

	template<typename Type>
	auto AddOnMaxValueChanged(const Type& Func)
	{
		return MaxValue.CallbackContainerHelper.AddOnValueChanged(Func);
	}

protected:

	friend FCharacterAttributes;

	friend FScoped_BaseProperty_SaveUpdate;

	FBaseProperty& GetCurrentProperty();

	FBaseProperty& GetMaxProperty();

	void Update();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty CurrentValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty MinValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseProperty MaxValue;

	TMap<FGuid, int32>ValueMap;

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
struct PLANET_API FCharacterAttributes final
{
	GENERATED_USTRUCT_BODY()

	FCharacterAttributes();

	virtual ~FCharacterAttributes();

	void ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Level = 1;

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

	TSharedPtr<FTalent_Base> TalentSPtr;
#pragma endregion

	// 基础属性
#pragma region 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet BaseAttackPower;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Penetration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet PercentPenetration;

	// 护甲
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
	FBasePropertySet MoveSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet RunningSpeedOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet RunningConsume;

	FGuid PropertuModify_GUID = FGuid::NewGuid();
};

struct PLANET_API FScopeCharacterAttributes
{
	FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes);
};

#pragma endregion CharacterAttributes