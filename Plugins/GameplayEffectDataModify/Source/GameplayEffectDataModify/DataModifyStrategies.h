// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemComponent.h"
#include "DataModify_GenericType.h"
#include "Components/ActorComponent.h"

class UCharacterAbilitySystemComponent;
class ACharacterBase;
class UAS_Character;

class IGameplayEffectDataModifyInterface;

struct FDataModify_key_compare;

struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;
struct FDataComposition;

class GAMEPLAYEFFECTDATAMODIFY_API IDataModifyInterface
{
public:
	friend FDataModify_key_compare;
	friend IGameplayEffectDataModifyInterface;

	IDataModifyInterface(
		int32 InPriority = 1
		);

	virtual ~IDataModifyInterface();;

	bool operator<(
		const IDataModifyInterface& RightValue
		) const;

protected:
private:
	// 越小的越先算, 100~200 用于基础功能
	int32 Priority = -1;

	int32 ID = -1;
};

struct FDataModify_key_compare
{
	bool operator()(
		const TSharedPtr<IDataModifyInterface>& lhs,
		const TSharedPtr<IDataModifyInterface>& rhs
		) const
	{
		return lhs->Priority < rhs->Priority;
	}
};

/**
 * 输出修正
 */
class GAMEPLAYEFFECTDATAMODIFY_API IOutputDataModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IOutputDataModifyInterface(
		int32 InPriority = 1
		);

	/**
	 * 
	 * @param Instigator 
	 * @param TargetCharacterPtr 
	 * @param NeedModifySet 
	 * @param RawDatas 
	 * @param NewDatas 
	 * @param AdditionalModifyAry 
	 * @return 本次修改完是否移除本【修正方式】
	 */
	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		);
};

/**
 * 输入修正
 */
class GAMEPLAYEFFECTDATAMODIFY_API IInputDataModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IInputDataModifyInterface(
		int32 InPriority = 1
		);

	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		);
};

/**
 * 数值修正
 * 比如移速是由
 * FDataComposition：多个来源组成，再加上对这些总数的乘数，有时也会强制把某个值改为特定的值，
 * 比如在魅惑的时候，会将移速固定在200以下
 */
class GAMEPLAYEFFECTDATAMODIFY_API IGetValueModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IGetValueModifyInterface(
		int32 InPriority = 1
		);

	virtual int32 GetValue(
		const FDataComposition& DataComposition,
		int32 PreviouValue
		) const;
};

/**
 * 消耗修正
 * 如我们需要将普通的Mana消耗，部分转为HP消耗
 */
class GAMEPLAYEFFECTDATAMODIFY_API IGostModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IGostModifyInterface(
		int32 InPriority = 1
		);

	virtual TMap<FGameplayTag, int32> GetCost(
		const TMap<FGameplayTag, int32>& Original,
		const TMap<FGameplayTag, int32>& CurrentOriginal
		) const;
};

/**
 * 范围大小修正
 */
class GAMEPLAYEFFECTDATAMODIFY_API IRangeModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IRangeModifyInterface(
		int32 InPriority = 1
		);

	virtual float GetRangeMagnitude() const;
};

/**
 * 持续时长修正
 */
class GAMEPLAYEFFECTDATAMODIFY_API IDurationModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IDurationModifyInterface(
		int32 InPriority = 1
		);

	virtual float GetDuration(
		const UAS_Character* AS_CharacterAttributePtr, 
		float Duration
		) const;
};

/**
 * 冷却修正
 */
class GAMEPLAYEFFECTDATAMODIFY_API ICooldownModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	ICooldownModifyInterface(
		int32 InPriority = 1
		);

	virtual int32 GetCooldown(
		const UAS_Character* AS_CharacterAttributePtr, 
		int32 Cooldown
		) const;
};

/**
 * 额外的效果修正
 * 类似LOL里面羊刀触发的攻击特效
 */
class GAMEPLAYEFFECTDATAMODIFY_API IExtraEffectModifyInterface : public IDataModifyInterface
{
public:
	using FPawnType = ACharacter;

	IExtraEffectModifyInterface(
		int32 InPriority = 1
		);

	virtual int32 GetExtraEffectCount() const;
};
