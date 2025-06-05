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


#pragma endregion
