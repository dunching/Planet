#pragma once

#include <functional>
#include <set>

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"

#include "ProxyProcessComponent.h"

#include "GroupManaggerInterface.h"
#include "PlanetAbilitySystemComponent.h"

class UCharacterAbilitySystemComponent;
class ACharacterBase;

struct FDataModify_key_compare;

class PLANET_API IDataModifyInterface
{
public:
	friend UCharacterAbilitySystemComponent;
	friend FDataModify_key_compare;

	IDataModifyInterface(
		int32 InPriority = 1
		);

	virtual ~IDataModifyInterface();;

	bool operator<(
		const IDataModifyInterface& RightValue
		) const;

protected:
	bool bIsOnceTime = false;

private:
	// 越小的越先算, 100~200 用于基础功能
	int32 Priority = -1;

	int32 ID = -1;
};

class PLANET_API IOutputDataModifyInterface : public IDataModifyInterface
{
public:
	IOutputDataModifyInterface(
		int32 InPriority = 1
		);

	// Return：本次修改完是否移除本【修正方式】
	virtual bool Modify(
		const TObjectPtr<ACharacterBase>& Instigator,
		const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
		);
};

class PLANET_API IInputDataModifyInterface : public IDataModifyInterface
{
public:
	IInputDataModifyInterface(
		int32 InPriority = 1
		);

	virtual bool Modify(
		const TObjectPtr<ACharacterBase>& Instigator,
		const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
		);
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

#pragma region 基础的折算

/**
 * 元素伤害的基础输入折算
 * 基于对方的穿透和自身的抗性，折算伤害数据
 */
class PLANET_API IInputData_BasicData_ModifyInterface : public IInputDataModifyInterface
{
public:
	IInputData_BasicData_ModifyInterface(
		int32 InPriority
		);

	virtual bool Modify(
		const TObjectPtr<ACharacterBase>& Instigator,
		const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
		) override;
};

/**
 * 元素伤害的护盾抵消输入折算
 */
class PLANET_API IInputData_Shield_ModifyInterface : public IInputDataModifyInterface
{
public:
	IInputData_Shield_ModifyInterface(
		int32 InPriority
		);

	virtual bool Modify(
		const TObjectPtr<ACharacterBase>& Instigator,
		const TObjectPtr<ACharacterBase>& TargetCharacterPtr,
		TSet<FGameplayTag>& NeedModifySet,
		TMap<FGameplayTag, float>& SetByCallerTagMagnitudes
		) override;
};

#pragma endregion
