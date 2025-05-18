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

	TMap<FGameplayTag, float> ValueMap;
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
	FMyGameplayAttributeData Max_Stamina;
	ATTRIBUTE_ACCESSORS(UAS_Character, Max_Stamina);
	
	// 体力,用于冲刺、奔跑
	UPROPERTY(ReplicatedUsing = OnRep_Stamina, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UAS_Character, Stamina);
	
	UFUNCTION()
	virtual void OnRep_Stamina(const FMyGameplayAttributeData& OldHealth);

	// 体力回复速率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Stamina_Replay;
	ATTRIBUTE_ACCESSORS(UAS_Character, Stamina_Replay);
	
	// 战斗资源，用于释放技能
	UPROPERTY(ReplicatedUsing = OnRep_Mana, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAS_Character, Mana);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Max_Mana;
	ATTRIBUTE_ACCESSORS(UAS_Character, Max_Mana);
	
	UFUNCTION()
	virtual void OnRep_Mana(const FMyGameplayAttributeData& OldHealth);

	// 战斗资源回复速率
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Mana_Replay;
	ATTRIBUTE_ACCESSORS(UAS_Character, Mana_Replay);
	
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
	UPROPERTY(ReplicatedUsing = OnRep_Shield, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UAS_Character, Shield);
	
	UFUNCTION()
	virtual void OnRep_Shield(const FMyGameplayAttributeData& OldHealth);

#pragma region 基础 属性：五行元素

	const int32 MaxElementalValue = 10000;
	
	const int32 MaxElementalLevel = 9;
	
	const int32 MaxElementalPenetration = 1000;
	
	const int32 MaxElementalPercentPenetration = 100;
	
	const int32 MaxElementalResistance = 10000;
	
	/**
	 * Value					元素强度
	 * Level					元素等级			0 3 6 9
	 * Penetration				元素抗性			0 ~ 1000
	 * Resistance				元素固定穿透		0 ~ 100
	 * PercentPenetration		元素穿透百分比
	 */
	 
	UPROPERTY(ReplicatedUsing = OnRep_MetalValue, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData MetalValue;
	ATTRIBUTE_ACCESSORS(UAS_Character, MetalValue);
	
	UFUNCTION()
	virtual void OnRep_MetalValue(const FMyGameplayAttributeData& OldHealth);

	UPROPERTY(ReplicatedUsing = OnRep_MetalLevel, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData MetalLevel;
	ATTRIBUTE_ACCESSORS(UAS_Character, MetalLevel);
	
	UFUNCTION()
	virtual void OnRep_MetalLevel(const FMyGameplayAttributeData& OldHealth);

	UPROPERTY(ReplicatedUsing = OnRep_MetalPenetration, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData MetalPenetration;
	ATTRIBUTE_ACCESSORS(UAS_Character, MetalPenetration);
	
	UFUNCTION()
	virtual void OnRep_MetalPenetration(const FMyGameplayAttributeData& OldHealth);

	UPROPERTY(ReplicatedUsing = OnRep_MetalPercentPenetration, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData MetalPercentPenetration;
	ATTRIBUTE_ACCESSORS(UAS_Character, MetalPercentPenetration);
	
	UFUNCTION()
	virtual void OnRep_MetalPercentPenetration(const FMyGameplayAttributeData& OldHealth);

	UPROPERTY(ReplicatedUsing = OnRep_MetalResistance, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData MetalResistance;
	ATTRIBUTE_ACCESSORS(UAS_Character, MetalResistance);

	UFUNCTION()
	virtual void OnRep_MetalResistance(const FMyGameplayAttributeData& OldHealth);

	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData WoodValue;
	ATTRIBUTE_ACCESSORS(UAS_Character, WoodValue);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData WoodLevel;
	ATTRIBUTE_ACCESSORS(UAS_Character, WoodLevel);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData WoodPenetration;
	ATTRIBUTE_ACCESSORS(UAS_Character, WoodPenetration);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData WoodPercentPenetration;
	ATTRIBUTE_ACCESSORS(UAS_Character, WoodPercentPenetration);
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	FMyGameplayAttributeData WoodResistance;
	ATTRIBUTE_ACCESSORS(UAS_Character, WoodResistance);
#pragma endregion
	
#pragma region 基础 属性：力道,根骨,身法,洞察,天资
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
#pragma endregion
	
};
