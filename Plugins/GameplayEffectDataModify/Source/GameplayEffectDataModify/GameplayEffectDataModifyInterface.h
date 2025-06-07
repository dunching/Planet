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
class IGetValueModifyInterface;
class IGostModifyInterface;

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

	/**
	 * 输出修正
	 * @param AllAssetTags 
	 * @param NeedModifySet 
	 * @param NewDatas 
	 * @param AdditionalModifyAry 
	 * @param ExecutionParams 
	 * @param OutExecutionOutput 
	 */
	virtual  void ModifyOutputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	/**
	 * 输入修正
	 * @param AllAssetTags 
	 * @param NeedModifySet 
	 * @param NewDatas 
	 * @param AdditionalModifyAry 
	 * @param ExecutionParams 
	 * @param OutExecutionOutput 
	 */
	virtual void ModifyInputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	/**
	 * 获取基础数据
	 * @param Spec 
	 * @param GameplayAttributeDataPtr 
	 * @return 
	 */
	float GetBaseValueMaps(
		const FGameplayAttributeData* GameplayAttributeDataPtr
		) const;

	/**
	 * 应用数据
	 * @param AllAssetTags 
	 * @param NeedModifySet 
	 * @param CustomMagnitudes 
	 * @param AdditionalModifyAry 
	 * @param ExecutionParams 
	 * @param OutExecutionOutput 
	 */
	virtual void ApplyInputData(
		const FGameplayTagContainer & AllAssetTags,
		TSet<FGameplayTag>& NeedModifySet,
		const TMap<FGameplayTag, float>& CustomMagnitudes,
		const TSet<EAdditionalModify>& AdditionalModifyAry,
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
		);

	/**
	 * 获取最终的消耗
	 * 如我们需要将将消耗降低30%
	 * @param CostMap 
	 * @return 
	 */
	TMap<FGameplayTag, int32>GetCost(const TMap<FGameplayTag, int32>&CostMap);

	/**
	 * 确认折算之后的消耗是否满足释放条件
	 * @param CostMap 
	 * @return 
	 */
	virtual bool CheckCost(const TMap<FGameplayTag, int32>&CostMap)const;	
	
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

	void AddGetValueModify(
		const TSharedPtr<IGetValueModifyInterface>& GAEventModifySPtr
		);

	void RemoveGetValueModify(
		const TSharedPtr<IGetValueModifyInterface>& GAEventModifySPtr
		);

	void AddGostModify(
		const TSharedPtr<IGostModifyInterface>& GAEventModifySPtr
		);

	void RemoveGostModify(
		const TSharedPtr<IGostModifyInterface>& GAEventModifySPtr
		);
#pragma endregion

protected:
	void UpdateValueMap();

	/**
	 * 从小到大
	 * 输出修正
	 */
	std::multiset<TSharedPtr<IOutputDataModifyInterface>, FDataModify_key_compare> OutputDataModifysMap;

	/**
	 * 输入修正
	 */
	std::multiset<TSharedPtr<IInputDataModifyInterface>, FDataModify_key_compare> InputDataModifysMap;

	/**
	 * 对于某类数值的获取修正
	 */
	std::multiset<TSharedPtr<IGetValueModifyInterface>, FDataModify_key_compare> GetValueModifysMap;

	/**
	 * "战斗资源"消耗的折算
	 */
	std::multiset<TSharedPtr<IGostModifyInterface>, FDataModify_key_compare> CostModifysMap;

	/**
	 * GameplayAttributeData的组成
	 * 如HP仅有 DataSource_Character 基础组成
	 * 而移速则会由 DataSource_Character 和减速时的减速buff 叠加负数
	 */
	TMap<const FGameplayAttributeData*, FDataComposition> ValueMap;
};
