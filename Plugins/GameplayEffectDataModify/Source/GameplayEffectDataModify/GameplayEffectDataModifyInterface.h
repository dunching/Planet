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
	 * 来源 FGameplayTag::EmptyTag为基础数据
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

enum class EUpdateValueType : uint8
{
	kPermanent_Addtive,
	kPermanent_Override,

	kTemporary_Data_Addtive,
	kTemporary_Data_Override,

	kTemporary_Percent_Addtive,
	kTemporary_Percent_Override,
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
	virtual void ModifyOutputData(
		const FGameplayTagContainer& AllAssetTags,
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
		const FGameplayTagContainer& AllAssetTags,
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
		const FGameplayTagContainer& AllAssetTags,
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
	TMap<FGameplayTag, int32> GetCost(
		const TMap<FGameplayTag, int32>& CostMap
		);

	/**
	 * 确认折算之后的消耗是否满足释放条件
	 * @param CostMap 
	 * @return 
	 */
	virtual bool CheckCost(
		const TMap<FGameplayTag, int32>& CostMap
		) const;

	virtual float GetDuration(
		const UAS_Character* AS_CharacterAttributePtr,
		float Duration
		) const;

	int32 GetCooldown(
		const UAS_Character* AS_CharacterAttributePtr,
		int32 Cooldown
		) const;

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

	void AddDurationModify(
		const TSharedPtr<IDurationModifyInterface>& GAEventModifySPtr
		);

	void RemoveDurationModify(
		const TSharedPtr<IDurationModifyInterface>& GAEventModifySPtr
		);

	void AddCooldownModify(
		const TSharedPtr<ICooldownModifyInterface>& GAEventModifySPtr
		);

	void RemoveCooldownModify(
		const TSharedPtr<ICooldownModifyInterface>& GAEventModifySPtr
		);
#pragma endregion

	/**
	 * 更新数据组成
	 * @param Value 该属性之后的值
	 * @param AllAssetTags 
	 * @param GameplayAttributeDataPtr 
	 */
	void UpdateTemporaryValue(
		const FGameplayTag& ModifyTypeTag,
		float Value,
		EUpdateValueType UpdateValueType,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

protected:
	void UpdateValueMap(
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

	/**
	 * 更新数据组成
	 * @param Tag 仅为 DataSource_Character
	 * @param Value 
	 * @param MinValue 仅GEData_ModifyType_BaseValue_Addtive、GEData_ModifyType_Immediate_Override 生效
	 * @param MaxValue 仅GEData_ModifyType_BaseValue_Addtive、GEData_ModifyType_Immediate_Override 生效
	 * @param Spec 
	 * @param GameplayAttributeDataPtr 
	 */
	void UpdatePermanentValue(
		float Value,
		int32 MinValue,
		int32 MaxValue,
		EUpdateValueType UpdateValueType,
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

	void UpdateTemporaryValue(
		const FGameplayTag& Tag,
		float Value,
		EUpdateValueType UpdateValueType,
		const FGameplayEffectSpec& Spec,
		const FGameplayAttributeData* GameplayAttributeDataPtr
		);

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
	 * 持续时长修正
	 */
	std::multiset<TSharedPtr<IDurationModifyInterface>, FDataModify_key_compare> DurationModifysMap;

	/**
	 * 冷却修正
	 */
	std::multiset<TSharedPtr<ICooldownModifyInterface>, FDataModify_key_compare> CooldownModifysMap;

	/**
	 * GameplayAttributeData的组成
	 * 如HP仅有 DataSource_Character 基础组成
	 * 而移速则会由 DataSource_Character 和减速时的减速buff 叠加负数
	 */
	TMap<const FGameplayAttributeData*, FDataComposition> ValueMap;
};
