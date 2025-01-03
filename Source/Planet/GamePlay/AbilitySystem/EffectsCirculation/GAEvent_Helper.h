#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "BaseData.h"

#include "GAEvent_Helper.generated.h"

class ACharacterBase;
class UInteractiveComponent;
class UCharacterAbilitySystemComponent;

struct FCharacterAttributes;

UENUM()
enum class EAttackEffectType : uint8
{
	kNone,
	kNormalAttackEffect,
	kAttackEffectAndRepel,
};

USTRUCT()
struct PLANET_API FGAEventData
{
	GENERATED_USTRUCT_BODY()

	FGAEventData();

	FGAEventData(
		TWeakObjectPtr<ACharacterBase> TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr
	);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	void SetBaseDamage(int32 Value);

	void AddWuXingDamage(EWuXingType WuXingType, int32 Value);

	bool GetIsHited() const;

	bool GetIsCriticalHited() const;

	// 是否是武器造成的伤害
	UPROPERTY()
	bool bIsWeaponAttack = false;

	// 是否是武器造成的伤害
	UPROPERTY()
	bool bIsCantEvade = false;

	UPROPERTY()
	// 是否会造成“受击”效果
	EAttackEffectType AttackEffectType = EAttackEffectType::kNone;

	// == 0则为 根据目标和tigger位置计算
	FVector RepelDirection = FVector::ZeroVector;

	int32 RepelDistance = -1;

	// 是否是“复活”技能
	UPROPERTY()
	bool bIsRespawn = false;

#pragma region
	// 这组数据 为基础值（即在SendEventImp之前填写的值） + 人物属性

	// 本次攻击的 穿透
	UPROPERTY()
	int32 AD_Penetration = 0;

	// 本次攻击的 百分比穿透
	UPROPERTY()
	int32 AD_PercentPenetration = 0;

	// 本次攻击的 命中率(0则为此次被闪避)，若大于 100，则为不可被闪避
	// 最终结算时：0 未命中 100 命中
	UPROPERTY()
	int32 HitRate = 0;

	// 本次攻击的 会心率(100则为此次被会心)
	UPROPERTY()
	int32 CriticalHitRate = 0;

	// 本次攻击的 会心伤害
	UPROPERTY()
	int32 CriticalDamage = 0;
#pragma endregion

	// 造成的真实伤害
	UPROPERTY()
	int32 TrueDamage = 0;

	// 造成的基础伤害
	UPROPERTY()
	int32 BaseDamage = 0;

	// 伤害分布：类型、等级、伤害量
	TSet<TTuple<EWuXingType, int32, int32>> ElementSet;

	UPROPERTY()
	FGameplayTag DataSource;

	// 是否 清空DataModify的 DataSource
	UPROPERTY()
	bool bIsClearData = false;

	// 是否 覆盖DataModify的 DataSource
	UPROPERTY()
	bool bIsOverlapData = false;

	// 直接修改的数据, 累加到 DataSource
	TMap<ECharacterPropertyType, FBaseProperty> DataModify;

	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;
};

UENUM()
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

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_GAEventType :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_GAEventType();

	FGameplayAbilityTargetData_GAEventType(
		EEventType InEventType
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	FGameplayAbilityTargetData_GAEventType* Clone() const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY()
	EEventType EventType = EEventType::kNormal;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_GAEventType> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_GAEventType>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_GASendEvent :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GASendEvent();

	FGameplayAbilityTargetData_GASendEvent(
		TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FGameplayAbilityTargetData_GASendEvent* Clone() const;

	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	UPROPERTY()
	FString TriggerCharacterName;

	UPROPERTY()
	TArray<FGAEventData> DataAry;

	FCallbackHandleContainer TrueDataDelagate;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_GASendEvent> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_GASendEvent>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_GAReceivedEvent :
	public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GAReceivedEvent();

	FGameplayAbilityTargetData_GAReceivedEvent(
		TWeakObjectPtr<ACharacterBase> TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FGameplayAbilityTargetData_GAReceivedEvent* Clone() const;

	TSharedPtr<FGameplayAbilityTargetData_GAReceivedEvent> Clone_SmartPtr() const;

	UPROPERTY()
	FGAEventData Data;

	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	FCallbackHandleContainer TrueDataDelagate;
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_GAReceivedEvent> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_GAReceivedEvent>
{
	enum
	{
		WithNetSerializer = true,
	};
};

struct FGAEventModify_key_compare;

class PLANET_API IGAEventModifyInterface
{
public:
	friend UCharacterAbilitySystemComponent;
	friend FGAEventModify_key_compare;

	IGAEventModifyInterface(int32 InPriority = 1);

	bool operator<(const IGAEventModifyInterface& RightValue) const;

protected:
	bool bIsOnceTime = false;

private:
	// 越小的越先算, 100~200 用于基础功能
	int32 Priority = -1;

	int32 ID = -1;
};

class PLANET_API IGAEventModifySendInterface : public IGAEventModifyInterface
{
public:
	IGAEventModifySendInterface(int32 InPriority = 1);

	// Return：本次修改完是否移除本【修正方式】
	virtual bool Modify(
		TMap<FGameplayTag, float>&	SetByCallerTagMagnitudes
		);
};

class PLANET_API IGAEventModifyReceivedInterface : public IGAEventModifyInterface
{
public:
	IGAEventModifyReceivedInterface(int32 InPriority = 1);

	virtual bool Modify(
		TMap<FGameplayTag, float>&	SetByCallerTagMagnitudes
	);
};

struct FGAEventModify_key_compare
{
	bool operator()(const TSharedPtr<IGAEventModifyInterface>& lhs,
	                const TSharedPtr<IGAEventModifyInterface>& rhs) const
	{
		return lhs->Priority < rhs->Priority;
	}
};
