// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExecutionCalculation.h"

#include "AS_Character.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct PLANET_API FMyGameplayAttributeData : public FGameplayAttributeData
{
	GENERATED_USTRUCT_BODY()
	
	virtual void SetBaseValue(float NewValue)override;

};

/**
 *
 */
UCLASS()
class PLANET_API UAS_Character : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 最大生命值
	UPROPERTY(ReplicatedUsing = OnRep_Max_HP, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Max_HP;
	ATTRIBUTE_ACCESSORS(UAS_Character, Max_HP);
	
	UFUNCTION()
	virtual void OnRep_Max_HP(const FMyGameplayAttributeData& OldHealth);

	// 生命值
	UPROPERTY(ReplicatedUsing = OnRep_HP, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData HP;
	ATTRIBUTE_ACCESSORS(UAS_Character, HP);
	
	UFUNCTION()
	virtual void OnRep_HP(const FMyGameplayAttributeData& OldHealth);

	// 生命值回复速率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData HP_Replay;
	ATTRIBUTE_ACCESSORS(UAS_Character, HP_Replay);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Max_PP;
	ATTRIBUTE_ACCESSORS(UAS_Character, Max_PP);
	
	// 体力,用于冲刺、奔跑
	UPROPERTY(ReplicatedUsing = OnRep_PP, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData PP;
	ATTRIBUTE_ACCESSORS(UAS_Character, PP);
	
	UFUNCTION()
	virtual void OnRep_PP(const FMyGameplayAttributeData& OldHealth);

	// 体力回复速率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData PP_Replay;
	ATTRIBUTE_ACCESSORS(UAS_Character, PP_Replay);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Max_Mana;
	ATTRIBUTE_ACCESSORS(UAS_Character, Max_Mana);
	
	// 法力值，用于释放技能
	UPROPERTY(ReplicatedUsing = OnRep_Mana, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAS_Character, Mana);
	
	UFUNCTION()
	virtual void OnRep_Mana(const FMyGameplayAttributeData& OldHealth);

	// 法力回复速率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Mana_Replay;
	ATTRIBUTE_ACCESSORS(UAS_Character, Mana_Replay);
	
	/**
	 * 内攻值
	 * 不可被闪避
	 * 可被对方内功减少伤害
	 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData AP;
	ATTRIBUTE_ACCESSORS(UAS_Character, AP);
	
	/**
	 * 外功值
	 * 可被闪避
	 * 可被抗性减少伤害
	 * 武器攻击时默认的加成
	 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData AD;
	ATTRIBUTE_ACCESSORS(UAS_Character, AD);
	
	// 攻击力穿透
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData AD_Penetration;
	ATTRIBUTE_ACCESSORS(UAS_Character, AD_Penetration);
	
	// 攻击力百分比穿透
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData AD_PercentPenetration;
	ATTRIBUTE_ACCESSORS(UAS_Character, AD_PercentPenetration);
	
	// AD 抗性
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData AD_Resistance;
	ATTRIBUTE_ACCESSORS(UAS_Character, AD_Resistance);
	
	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UAS_Character, MoveSpeed);
	
	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FMyGameplayAttributeData& OldHealth);

	// 会心伤害
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UAS_Character, CriticalDamage);
	
	// 会心率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData CriticalHitRate;
	ATTRIBUTE_ACCESSORS(UAS_Character, CriticalHitRate);
	
	// 命中率
	// 目标 闪避值 为0时命中的概率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData HitRate;
	ATTRIBUTE_ACCESSORS(UAS_Character, HitRate);
	
	// 闪避几率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Evade;
	ATTRIBUTE_ACCESSORS(UAS_Character, Evade);
	
	// 攻击速度、技能释放速度
	UPROPERTY(ReplicatedUsing = OnRep_PerformSpeed, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData PerformSpeed;
	ATTRIBUTE_ACCESSORS(UAS_Character, PerformSpeed);
	
	UFUNCTION()
	virtual void OnRep_PerformSpeed(const FMyGameplayAttributeData& OldHealth);

	// 当前护盾值
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UAS_Character, Shield);
	
	// 基础 属性：力道、根骨、身法、洞察、天资
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData LiDao;
	ATTRIBUTE_ACCESSORS(UAS_Character, LiDao);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData GenGu;
	ATTRIBUTE_ACCESSORS(UAS_Character, GenGu);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData ShenFa;
	ATTRIBUTE_ACCESSORS(UAS_Character, ShenFa);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData DongCha;
	ATTRIBUTE_ACCESSORS(UAS_Character, DongCha);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData TianZi;
	ATTRIBUTE_ACCESSORS(UAS_Character, TianZi);
	
};
