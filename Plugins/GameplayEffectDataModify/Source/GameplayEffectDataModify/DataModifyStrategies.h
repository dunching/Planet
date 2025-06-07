// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemComponent.h"
#include "DataModify_GenericType.h"
#include "Components/ActorComponent.h"

class UCharacterAbilitySystemComponent;
class ACharacterBase;

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
