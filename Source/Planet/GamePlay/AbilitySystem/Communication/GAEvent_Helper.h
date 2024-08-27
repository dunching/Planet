

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "GenerateType.h"
#include "SceneElement.h"

class ACharacterBase;
class UInteractiveComponent;
class UInteractiveBaseGAComponent;

struct FCharacterAttributes;

struct FGAEventData
{
	FGAEventData(
		TWeakObjectPtr<ACharacterBase>  TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	// 是否是武器造成的伤害
	bool bIsWeaponAttack = false;
	
	// 是否会造成“受击”效果
	bool bIsMakeAttackEffect = false;

	// 本次攻击的 穿透
	int32 Penetration = 0;

	// 本次攻击的 百分比穿透
	int32 PercentPenetration = 0;

	// 本次攻击的 命中率(0则为此次被闪避)
	int32 HitRate = 0;

	// 本次攻击的 会心率(100则为此次被会心)
	int32 CriticalHitRate = 0;

	// 本次攻击的 会心伤害
	int32 CriticalDamage = 0;

	// 造成的真实伤害
	int32 TrueDamage = 0;

	// 造成的基础伤害
	int32 BaseDamage = 0;

	// 伤害分布：类型、等级、伤害量
	TSet<TTuple<EWuXingType, int32, int32>>ElementSet;

	// 回复：HP
	int32 HP = 0;

	// 回复：PP
	int32 PP = 0;

	void SetBaseDamage(int32 Value);

	void AddWuXingDamage(EWuXingType WuXingType, int32 Value);

	FGameplayTag DataSource;

	// 是否 清空DataModify的 DataSource
	bool bIsClearData = false;

	// 直接修改的数据
	TMap<ECharacterPropertyType, FBaseProperty>DataModify;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;
};

struct FGameplayAbilityTargetData_GAEventType : public FGameplayAbilityTargetData
{
	enum class EEventType : uint8
	{
		// 人物受“击时”收到的数据
		kNormal,
		// 人物添加“某些标签”时收到的数据
		kRootMotion,
		// 人物获得“周期性增加属性”时收到的数据，通常是使用消耗品
		kPeriodic_PropertyModify,
		// 人物执行“RootMoxing”时收到的数据
		kPeriodic_StateTagModify,
	};

	FGameplayAbilityTargetData_GAEventType();

	FGameplayAbilityTargetData_GAEventType(
		EEventType InEventType
	);

	FGameplayAbilityTargetData_GAEventType* Clone()const;

	EEventType EventType = EEventType::kNormal;
};

struct FGameplayAbilityTargetData_GASendEvent : public FGameplayAbilityTargetData
{
	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GASendEvent(
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	FGameplayAbilityTargetData_GASendEvent* Clone()const;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	TArray<FGAEventData> DataAry;

	FCallbackHandleContainer TrueDataDelagate;

};

struct FGameplayAbilityTargetData_GAReceivedEvent : public FGameplayAbilityTargetData
{
	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GAReceivedEvent(
		TWeakObjectPtr<ACharacterBase>  TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	FGameplayAbilityTargetData_GAReceivedEvent* Clone()const;

	TSharedPtr<FGameplayAbilityTargetData_GAReceivedEvent> Clone_SmartPtr()const;

	FGAEventData Data;

	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	FCallbackHandleContainer TrueDataDelagate;
};

struct FGAEventModify_key_compare;

class PLANET_API IGAEventModifyInterface
{
public:

	friend UInteractiveBaseGAComponent;
	friend FGAEventModify_key_compare;

	IGAEventModifyInterface(int32 InPriority = 1);

	bool operator<(const IGAEventModifyInterface& RightValue)const;

private:

	// 越小的越先算
	int32 Priority = -1;

	int32 ID = -1;

};

class PLANET_API IGAEventModifySendInterface : public IGAEventModifyInterface
{
public:

	IGAEventModifySendInterface(int32 InPriority = 1);

	virtual void Modify(FGameplayAbilityTargetData_GASendEvent& OutGameplayAbilityTargetData_GAEvent);

};

class PLANET_API IGAEventModifyReceivedInterface : public IGAEventModifyInterface
{
public:

	IGAEventModifyReceivedInterface(int32 InPriority = 1);

	virtual void Modify(FGameplayAbilityTargetData_GAReceivedEvent& OutGameplayAbilityTargetData_GAEvent);

};

struct FGAEventModify_key_compare
{
	bool operator()(const TSharedPtr<IGAEventModifyInterface>& lhs, const TSharedPtr<IGAEventModifyInterface>& rhs) const
	{
		return lhs->Priority < rhs->Priority;
	}
};