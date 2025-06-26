#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

#include "ProxyProcessComponent.h"

#include "DataModifyStrategies.h"

class UCharacterAbilitySystemComponent;
class ACharacterBase;

class PLANET_API IOutputData_ModifyInterface_Base : public IOutputDataModifyInterface
{
public:
	using FPawnType = ACharacterBase;

	IOutputData_ModifyInterface_Base(
		int32 InPriority
		);
};

class PLANET_API IInputData_ModifyInterface_Base : public IInputDataModifyInterface
{
public:
	using FPawnType = ACharacterBase;

	IInputData_ModifyInterface_Base(
		int32 InPriority
		);
};

#pragma region 基础的折算

/**
 * 会心率、会心伤害确认
 */
class PLANET_API IOutputData_ProbabilityConfirmation_ModifyInterface : public IOutputData_ModifyInterface_Base
{
public:
	IOutputData_ProbabilityConfirmation_ModifyInterface(
		int32 InPriority
		);

	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		) override;
};

/**
 * 命中率确认
 */
class PLANET_API IInputData_ProbabilityConfirmation_ModifyInterface : public IInputData_ModifyInterface_Base
{
public:
	IInputData_ProbabilityConfirmation_ModifyInterface(
		int32 InPriority
		);

	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		) override;
};

/**
 * 元素伤害的基础输入折算
 * 基于对方的穿透和自身的抗性，折算伤害数据
 */
class PLANET_API IInputData_BasicData_ModifyInterface : public IInputData_ModifyInterface_Base
{
public:
	IInputData_BasicData_ModifyInterface(
		int32 InPriority
		);

	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		) override;
};

/**
 * 元素伤害的护盾抵消输入折算
 */
class PLANET_API IInputData_Shield_ModifyInterface : public IInputData_ModifyInterface_Base
{
public:
	IInputData_Shield_ModifyInterface(
		int32 InPriority
		);

	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		) override;
};

/**
 * 
 */
class PLANET_API IGetValueGenericcModifyInterface : public IGetValueModifyInterface
{
public:
	IGetValueGenericcModifyInterface(
		int32 InPriority
		);

	virtual int32 GetValue(
		const FDataComposition& DataComposition,
		int32 PreviouValue
		) const override;
};

/**
 * 
 */
class PLANET_API IBasicGostModifyInterface : public IGostModifyInterface
{
public:
	IBasicGostModifyInterface(
		int32 InPriority
		);

	virtual TMap<FGameplayTag, int32> GetCost(
		const TMap<FGameplayTag, int32>& Original,
		const TMap<FGameplayTag, int32>& CurrentOriginal
		) const override;
};

/**
 * 
 */
class PLANET_API IBasicDurationModifyInterface : public IDurationModifyInterface
{
public:
	IBasicDurationModifyInterface(
		int32 InPriority
		);

	virtual float GetDuration(
		const UAS_Character* AS_CharacterAttributePtr, 
		float Duration
		) const override;
};

/**
 * 
 */
class PLANET_API IBasicCooldownModifyInterface : public ICooldownModifyInterface
{
public:
	IBasicCooldownModifyInterface(
		int32 InPriority
		);

	virtual int32 GetCooldown(
		const UAS_Character* AS_CharacterAttributePtr, 
		int32 Cooldown
		) const override;
};

#pragma endregion

#pragma region 通用的修正

/**
 * 接下来的伤害修正输出的次数
 */
class PLANET_API IOutputData_MultipleDamega_ModifyInterface : public IOutputData_ModifyInterface_Base
{
public:
	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

	IOutputData_MultipleDamega_ModifyInterface(
		int32 InPriority,
		int32 InCount,
		float InMultiple
		);

	virtual bool Modify(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& NewDatas,
		TSet<EAdditionalModify>& AdditionalModifyAry
		) override;

	float Multiple = 1.f;

	int32 Count = 1;

	int32 CurrentCount = 0;
};

/**
 * 将消耗的Mana部分替换为其他资源
 */
class PLANET_API IGostModify_ReplaceWithOther_Interface : public IGostModifyInterface
{
public:
	IGostModify_ReplaceWithOther_Interface(
		int32 InPriority,
		FGameplayTag InCostAttributeTag,
		int32 InManaPercent,
		int32 InNewResourcePercent
		);

	virtual TMap<FGameplayTag, int32> GetCost(
		const TMap<FGameplayTag, int32>& Original,
		const TMap<FGameplayTag, int32>& CurrentOriginal
		) const override;

	FGameplayTag CostAttributeTag;

	int32 ManaPercent = 30;

	int32 NewResourcePercent = 100;
};

/**
 * 将消耗的Mana按倍率进行乘积
 */
class PLANET_API IGostModify_Multiple_Interface : public IGostModifyInterface
{
public:
	IGostModify_Multiple_Interface(
		int32 InPriority,
		float InMultiple
		);

	virtual TMap<FGameplayTag, int32> GetCost(
		const TMap<FGameplayTag, int32>& Original,
		const TMap<FGameplayTag, int32>& CurrentOriginal
		) const override;

	float Multiple = 1.f;
};

#pragma endregion

enum class EOutputModifyOrder : uint16
{
	kProbabilityConfirmation = 150,
	
	kSkill_Passive_MultipleDamega = 151,
};

enum class EInputModifyOrder: uint16
{
	kProbabilityConfirmation = 150,
	kBasicData = 151,
	kShield = 152,
};

enum class EGetValueModifyOrder: uint16
{
	kGenericc = 150,
};

enum class EGostModifyOrder: uint16
{
	kBasic = 150,
	
	kSkill_Passive_ManaCostModify_HP = 151,
};

enum class EDurationModifyOrder: uint16
{
	kBasic = 150,
};

enum class ECooldownModifyOrder: uint16
{
	kBasic = 150,
};
