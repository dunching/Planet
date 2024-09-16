

#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "GenerateType.h"
#include "SceneElement.h"

#include "GAEvent_Helper.generated.h"

class ACharacterBase;
class UInteractiveComponent;
class UBaseFeatureGAComponent;

struct FCharacterAttributes;

USTRUCT()
struct PLANET_API FGAEventData
{
	GENERATED_USTRUCT_BODY()

	FGAEventData();

	FGAEventData(
		TWeakObjectPtr<ACharacterBase>  TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	
	UPROPERTY()
	// 是否是武器造成的伤害
	bool bIsWeaponAttack = false;
	
	UPROPERTY()
	// 是否会造成“受击”效果
	bool bIsMakeAttackEffect = false;
	
	UPROPERTY()
	// 本次攻击的 穿透
	int32 Penetration = 0;
	
	UPROPERTY()
	// 本次攻击的 百分比穿透
	int32 PercentPenetration = 0;
	
	UPROPERTY()
	// 本次攻击的 命中率(0则为此次被闪避)
	int32 HitRate = 0;
	
	UPROPERTY()
	// 本次攻击的 会心率(100则为此次被会心)
	int32 CriticalHitRate = 0;
	
	UPROPERTY()
	// 本次攻击的 会心伤害
	int32 CriticalDamage = 0;
	
	UPROPERTY()
	// 造成的真实伤害
	int32 TrueDamage = 0;
	
	UPROPERTY()
	// 造成的基础伤害
	int32 BaseDamage = 0;
	
	// 伤害分布：类型、等级、伤害量
	TSet<TTuple<EWuXingType, int32, int32>>ElementSet;
	
	UPROPERTY()
	// 回复：HP
	int32 HP = 0;
	
	UPROPERTY()
	// 回复：PP
	int32 PP = 0;

	void SetBaseDamage(int32 Value);

	void AddWuXingDamage(EWuXingType WuXingType, int32 Value);
	
	UPROPERTY()
	FGameplayTag DataSource;

	// 是否 清空DataModify的 DataSource
	UPROPERTY()
	bool bIsClearData = false;
	
	// 是否 覆盖DataModify的 DataSource
	UPROPERTY()
	bool bIsOverlapData = false;
	
	// 直接修改的数据,累加到 DataSource
	TMap<ECharacterPropertyType, FBaseProperty>DataModify;
	
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
struct PLANET_API FGameplayAbilityTargetData_GAEventType : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityTargetData_GAEventType();

	FGameplayAbilityTargetData_GAEventType(
		EEventType InEventType
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	FGameplayAbilityTargetData_GAEventType* Clone()const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	
	UPROPERTY()
	EEventType EventType = EEventType::kNormal;
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_GAEventType> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_GAEventType>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_GASendEvent : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GASendEvent();

	FGameplayAbilityTargetData_GASendEvent(
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FGameplayAbilityTargetData_GASendEvent* Clone()const;

	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;
	
	UPROPERTY()
	FString TriggerCharacterName;
	
	UPROPERTY()
	TArray<FGAEventData> DataAry;

	FCallbackHandleContainer TrueDataDelagate;

};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_GASendEvent> :
	public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_GASendEvent>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FGameplayAbilityTargetData_GAReceivedEvent : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()

	using FCallbackHandleContainer = TCallbackHandleContainer<void(ACharacterBase*, const FGAEventData&)>;

	FGameplayAbilityTargetData_GAReceivedEvent();

	FGameplayAbilityTargetData_GAReceivedEvent(
		TWeakObjectPtr<ACharacterBase>  TargetCharacterPtr,
		TWeakObjectPtr<ACharacterBase>  TriggerCharacterPtr
	);

	virtual UScriptStruct* GetScriptStruct() const override;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	FGameplayAbilityTargetData_GAReceivedEvent* Clone()const;

	TSharedPtr<FGameplayAbilityTargetData_GAReceivedEvent> Clone_SmartPtr()const;
	
	UPROPERTY()
	FGAEventData Data;
	
	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> TriggerCharacterPtr = nullptr;

	FCallbackHandleContainer TrueDataDelagate;
};

template<>
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

	friend UBaseFeatureGAComponent;
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