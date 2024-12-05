// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "BaseData.h"
#include "GAEvent_Helper.h"
#include "GameplayTagContainer.h"

#include "CharacterAttibutes.generated.h"

class FTalent_Base;

#pragma region CharacterAttributes

struct FCharacterAttributes;

struct FPropertySettlementModify
{
	FPropertySettlementModify(int32 InPriority = 1);

	virtual int32 SettlementModify(const TMap<FGameplayTag, int32>& ValueMap)const;

	// 仅使用优先级最高的
	// 越大的越先算
	int32 Priority = -1;

	// -1 为无效
	int32 ID = -1;

private:

};

struct FPropertySettlementModify_Compare
{
	bool operator()(
		const TSharedPtr<FPropertySettlementModify>& lhs, 
		const TSharedPtr<FPropertySettlementModify>& rhs
		) const
	{
		return lhs->Priority > rhs->Priority;
	}
};

USTRUCT(BlueprintType)
struct PLANET_API FBasePropertySet
{
	GENERATED_USTRUCT_BODY()

public:

	FBasePropertySet();

	void AddCurrentValue(int32 NewValue, const FGameplayTag& DataSource);

	void RemoveCurrentValue(const FGameplayTag& DataSource);

	void SetCurrentValue(int32 NewValue, const FGameplayTag& DataSource);

	// 设置 ValueMap 下的“值总和”为指定的值
	void SetValue(int32 NewValue);

	int32 GetCurrentValue()const;

	const FBaseProperty& GetCurrentProperty()const;

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

	int32 AddSettlementModify(const TSharedPtr<FPropertySettlementModify>& PropertySettlementModify);

	int32 UpdateSettlementModify(const TSharedPtr<FPropertySettlementModify>& PropertySettlementModify);

	void RemoveSettlementModify(const TSharedPtr<FPropertySettlementModify>& PropertySettlementModify);

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

	TMap<FGameplayTag, int32>ValueMap;

	// 只会使用优先级最高的.
	// 有一个默认的结算类
	std::multiset<TSharedPtr<FPropertySettlementModify>, FPropertySettlementModify_Compare>PropertySettlementModifySet;

};

USTRUCT(BlueprintType)
struct PLANET_API FCharacterAttributes final
{
	GENERATED_USTRUCT_BODY()

	using FProcessedGAEvent = TCallbackHandleContainer<void(const FGameplayAbilityTargetData_GAReceivedEvent&)>;

	FCharacterAttributes();

	virtual ~FCharacterAttributes();

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	void InitialData();

	void ProcessGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

	bool Identical(const FCharacterAttributes* Other, uint32 PortFlags) const;

	bool operator==(const FCharacterAttributes& RightValue) const;

	const FBasePropertySet& GetHPReply()const;

	const FBasePropertySet& GetPPReply()const;

	FProcessedGAEvent ProcessedGAEvent;

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

	// 五行、天赋属性
#pragma region 
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

	TSharedPtr<FTalent_Base> TalentSPtr;
#pragma endregion

	// 基础属性
#pragma region 

	// 攻击力
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD;

	// 攻击力穿透
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD_Penetration;

	// 攻击力百分比穿透
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD_PercentPenetration;
	
	// AD 抗性
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AD_Resistance;
	
	// 元素强度
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP;

	// 元素穿透
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP_Penetration;

	// 元素百分比穿透
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP_PercentPenetration;

	// 元素 抗性
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet AP_Resistance;

	// 攻击速度、技能释放速度
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet GAPerformSpeed;

	// 当前护盾值
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Shield;
	
	// 生命值
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet HP;

	// 生命值回复速率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet HP_Replay;

	// 体力,用于冲刺、奔跑
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet PP;

	// 体力回复速率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet PP_Replay;
	
	// 法力值，用于释放技能
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Mana;

	// 法力速率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Mana_Replay;

	// 闪避几率
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBasePropertySet Evade;

	// 命中率
	// 目标 闪避值 为0时命中的概率
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

private:

	bool bIsNotChanged = true;

};

template<>
struct TStructOpsTypeTraits< FCharacterAttributes > :
	public TStructOpsTypeTraitsBase2< FCharacterAttributes >
{
	enum
	{
		WithNetSerializer = true,
//		WithIdenticalViaEquality = true,
		WithIdentical = true,
	};
};

struct PLANET_API FScopeCharacterAttributes
{
	FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes);
};

#pragma endregion CharacterAttributes
