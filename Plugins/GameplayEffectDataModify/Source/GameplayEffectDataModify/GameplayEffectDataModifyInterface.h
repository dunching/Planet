// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "DataModify_GenericType.h"
#include "DataModifyStrategies.h"

#include "GameplayEffectDataModifyInterface.generated.h"

class ACharacter;
class UPlanetAbilitySystemComponent;

class IInputDataModifyInterface;
class IOutputDataModifyInterface;

struct FGameplayEffectCustomExecutionParameters;
struct FGameplayEffectCustomExecutionOutput;
struct FGameplayTagContainer;
struct FGameplayTag;
struct FGameplayEffectSpec;
struct FGameplayAttributeData;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGameplayEffectDataModifyInterface : public UInterface
{
	GENERATED_BODY()
};

USTRUCT()
struct GAMEPLAYEFFECTDATAMODIFY_API FDataComposition
{
	GENERATED_USTRUCT_BODY()

	void Empty();
	
	/**
	 * 数据的基础组成
	 * 来源
	 * 值
	 */
	TMap<FGameplayTag, float> DataMap;
	
	/**
	 * 数据的百分比组成
	 * 来源
	 * 修正值 Percent
	 */
	TMap<FGameplayTag, float> MagnitudeMap;
};

class GAMEPLAYEFFECTDATAMODIFY_API IGameplayEffectDataModifyInterface
{
	GENERATED_BODY()

public:
	using FPawnType = ACharacter;

#pragma region 输入和输出得修正

	virtual  void ModifyOutputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	virtual void ModifyInputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	virtual void ApplyInputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	void AddOutputModify(
		const TSharedPtr<IOutputDataModifyInterface>& GAEventModifySPtr
		);

	void RemoveOutputModify(
		const TSharedPtr<IOutputDataModifyInterface>& GAEventModifySPtr
		);

	void AddInputModify(
		const TSharedPtr<IInputDataModifyInterface>& GAEventModifySPtr
		);

	void RemoveInputModify(
		const TSharedPtr<IInputDataModifyInterface>& GAEventModifySPtr
		);
#pragma endregion

protected:
	/**
	 * 获取基础数据
	 * @param Spec 
	 * @param GameplayAttributeDataPtr 
	 * @return 
	 */
	float GetBaseValueMaps(
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		) const;

	void UpdateValueMap();

	// 从小到大
	std::multiset<TSharedPtr<IOutputDataModifyInterface>, FDataModify_key_compare> OutputDataModifysMap;

	std::multiset<TSharedPtr<IInputDataModifyInterface>, FDataModify_key_compare> InputDataModifysMap;

	/**
	 * GameplayAttributeData的组成
	 * 如HP仅有 DataSource_Character 基础组成
	 * 而移速则会由 DataSource_Character 和减速时的减速buff 叠加负数
	 */
	TMap<const FGameplayAttributeData*, FDataComposition> ValueMap;
};
