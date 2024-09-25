// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"

#include "GenerateType.h"
#include "BaseData.h"

#include "SceneElement.generated.h"

struct FGameplayAbilityTargetData_RegisterParam;
struct FTableRowUnit_CommonCooldownInfo;
struct FTableRowUnit;
struct FTableRowUnit_WeaponExtendInfo;
struct FTableRowUnit_ActiveSkillExtendInfo;
struct FTableRowUnit_PassiveSkillExtendInfo;
struct FTableRowUnit_WeaponSkillExtendInfo;
struct FTableRowUnit_CharacterInfo;
struct FTableRowUnit_Consumable;
struct FTableRowUnit_PropertyEntrys;
class AToolUnitBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class USkill_Consumable_Generic;
class AConsumable_Base;
class IPlanetControllerInterface;
class APlanetPlayerState;
class APlanetGameMode;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter;
class UHoldingItemsComponent;
struct FCharacterProxy;

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneUnitContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

enum struct ECharacterPropertyType : uint8;

FTableRowUnit_CommonCooldownInfo* GetTableRowUnit_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag);

struct PLANET_API IUnit_Cooldown
{
public:

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const = 0;

	virtual bool CheckCooldown()const = 0;

	virtual void AddCooldownConsumeTime(float NewTime) = 0;

	virtual void FreshUniqueCooldownTime() = 0;

	virtual void OffsetCooldownTime() = 0;

	virtual void ApplyCooldown() = 0;

};

// 场景内的对象代理
// 通用数据记录在DataTable，变化数据记录在对象内
// 序列化&反序列化
USTRUCT()
struct PLANET_API FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend FProxy_FASI_Container;
	friend UHoldingItemsComponent;
	friend APlanetGameMode;

	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual void InitialUnit();

	// 激活
	virtual bool Active();

	//  取消 激活
	virtual void Cancel();

	// 装备至插槽
	virtual void Allocation();

	// 从插槽移除
	virtual void UnAllocation();

	IDType GetID()const;

	FGameplayTag GetUnitType()const;

	TSoftObjectPtr<UTexture2D> GetIcon()const;

	// 
	FString GetUnitName()const;

	TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)> OnAllocationCharacterUnitChanged;

	TWeakPtr<FCharacterProxy> GetAllocationCharacterUnit()const;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr);

	ACharacterBase* GetProxyCharacter()const;

	ACharacterBase* GetAllocationCharacter()const;

	void Update2Client();

	TSharedPtr<FBasicProxy> GetThisSPtr()const;

	// 这个物品所在的对象
	TWeakPtr<FCharacterProxy> OwnerCharacterUnitPtr = nullptr;

protected:

	FTableRowUnit* GetTableRowUnit()const;

	UPROPERTY(Transient)
	FGameplayTag UnitType = FGameplayTag::EmptyTag;

	// 这个物品被分配给的对象
	TWeakPtr<FCharacterProxy> AllocationCharacterUnitPtr = nullptr;

	IDType ID;

private:

};

template<>
struct TStructOpsTypeTraits<FBasicProxy> :
	public TStructOpsTypeTraitsBase2<FBasicProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FCoinProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend FProxy_FASI_Container;
	friend UHoldingItemsComponent;

	FCoinProxy();

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

protected:

	int32 Num = 1;

};

USTRUCT()
struct PLANET_API FConsumableProxy :
	public FBasicProxy,
	public IUnit_Cooldown
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend FProxy_FASI_Container;
	friend UHoldingItemsComponent;

	FConsumableProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual bool Active()override;

	FTableRowUnit_Consumable* GetTableRowUnit_Consumable()const;

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const override;

	virtual bool CheckCooldown()const override;

	virtual void AddCooldownConsumeTime(float NewTime)override;

	virtual void FreshUniqueCooldownTime()override;

	virtual void ApplyCooldown()override;

	virtual void OffsetCooldownTime()override;

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;

protected:

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

	int32 Num = 1;

};

template<>
struct TStructOpsTypeTraits<FConsumableProxy> :
	public TStructOpsTypeTraitsBase2<FConsumableProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FToolProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend UHoldingItemsComponent;

	FToolProxy();

	int32 GetNum()const;

	int32 DamageDegree = 0;

protected:

	int32 Num = 1;

};

USTRUCT()
struct PLANET_API FCharacterProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	using FPawnType = ACharacterBase;

	FCharacterProxy();

	virtual void InitialUnit()override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	FTableRowUnit_CharacterInfo* GetTableRowUnit_CharacterInfo()const;

	// 解除这个类下AddToRoot的对象
	void RelieveRootBind();

	TWeakObjectPtr<FPawnType> ProxyCharacterPtr = nullptr;

protected:

};

template<>
struct TStructOpsTypeTraits<FCharacterProxy> :
	public TStructOpsTypeTraitsBase2<FCharacterProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FSkillProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend UHoldingItemsComponent;

	FSkillProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const;

	// 装备至插槽
	virtual void Allocation()override;

	// 从插槽移除
	virtual void UnAllocation()override;

	TArray<USkill_Base*> GetGAInstAry()const;

	USkill_Base* GetGAInst()const;

	FGameplayAbilitySpecHandle GetGAHandle()const;

	int32 Level = 1;

protected:

	void RegisterSkill();

	void UnRegisterSkill();

	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;

};

template<>
struct TStructOpsTypeTraits<FSkillProxy> :
	public TStructOpsTypeTraitsBase2<FSkillProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FPassiveSkillProxy : public FSkillProxy
{
	GENERATED_USTRUCT_BODY()

public:

	FPassiveSkillProxy();

	virtual void InitialUnit()override;

	// 装备至插槽
	virtual void Allocation()override;

	// 从插槽移除
	virtual void UnAllocation()override;

	FTableRowUnit_PassiveSkillExtendInfo* GetTableRowUnit_PassiveSkillExtendInfo()const;

	FTableRowUnit_PropertyEntrys* GetMainPropertyEntry()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

	// 元素词条
	FGameplayTag ElementPropertyEntry;

	// 主词条
	FGameplayTag MainPropertyEntry;
	
	// 副词条
	FGameplayTag SecondPropertyEntry;

protected:

};

USTRUCT()
struct PLANET_API FActiveSkillProxy :
	public FSkillProxy,
	public IUnit_Cooldown
{
	GENERATED_USTRUCT_BODY()

public:

	FActiveSkillProxy();

	virtual bool Active()override;

	virtual void Cancel()override;

	FTableRowUnit_ActiveSkillExtendInfo* GetTableRowUnit_ActiveSkillExtendInfo()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const override;

	virtual bool CheckCooldown()const override;

	virtual void AddCooldownConsumeTime(float NewTime)override;

	virtual void FreshUniqueCooldownTime()override;

	virtual void ApplyCooldown()override;

	virtual void OffsetCooldownTime()override;

protected:

};

USTRUCT()
struct PLANET_API FTalentSkillProxy : public FSkillProxy
{
	GENERATED_USTRUCT_BODY()

public:

	FTalentSkillProxy();

protected:

};

USTRUCT()
struct PLANET_API FWeaponSkillProxy : public FSkillProxy
{
	GENERATED_USTRUCT_BODY()

public:

	FWeaponSkillProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)override;

	virtual bool Active()override;

	virtual void Cancel()override;

	FTableRowUnit_WeaponSkillExtendInfo* GetTableRowUnit_WeaponSkillExtendInfo()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

protected:

};

template<>
struct TStructOpsTypeTraits<FWeaponSkillProxy> :
	public TStructOpsTypeTraitsBase2<FWeaponSkillProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FWeaponProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;

	FWeaponProxy();

	virtual void InitialUnit()override;

	virtual bool Active()override;

	virtual void Cancel()override;

	// 装备至插槽
	virtual void Allocation()override;

	// 从插槽移除
	virtual void UnAllocation()override;

	virtual void SetAllocationCharacterUnit(const TSharedPtr<FCharacterProxy>& InAllocationCharacterUnitPtr)override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	FTableRowUnit_WeaponExtendInfo* GetTableRowUnit_WeaponExtendInfo()const;

	// 切换至当前武器
	void ActiveWeapon();

	// 收回武器
	void RetractputWeapon();

	// 主词条
	FTableRowUnit_PropertyEntrys* GetMainPropertyEntry()const;

	int32 GetMaxAttackDistance()const;

	// 注意：因为不能确定 “复制顺序”，所以这里不能用 WeakPtr
	TSharedPtr<FWeaponSkillProxy>FirstSkill;

protected:

	UPROPERTY(Transient)
	int32 MaxAttackDistance = 100;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

};

template<>
struct TStructOpsTypeTraits<FWeaponProxy> :
	public TStructOpsTypeTraitsBase2<FWeaponProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
