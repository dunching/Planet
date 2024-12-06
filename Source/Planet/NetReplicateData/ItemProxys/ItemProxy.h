// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"

#include "TemplateHelper.h"

#include "ItemProxy.generated.h"

struct FGameplayAbilityTargetData_RegisterParam;
struct FTableRowUnit_CommonCooldownInfo;
struct FTableRowUnit;
struct FTableRowUnit_WeaponExtendInfo;
struct FTableRowUnit_ActiveSkillExtendInfo;
struct FTableRowUnit_PassiveSkillExtendInfo;
struct FTableRowUnit_WeaponSkillExtendInfo;
struct FTableRowUnit_CharacterGrowthAttribute;
struct FTableRowUnit_Consumable;
struct FTableRowUnit_PropertyEntrys;
struct FTableRowUnit_CharacterType;
class UTexture2D;
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
class AHumanCharacter_AI;
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

	using FOnAllocationCharacterUnitChanged =
		TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)>;

	friend FSceneUnitContainer;
	friend FProxy_FASI_Container;
	friend UHoldingItemsComponent;
	friend APlanetGameMode;

	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual void InitialUnit();

	// 通过远程的更新客户端的数据
	void UpdateByRemote(const TSharedPtr<FBasicProxy>& RemoteSPtr);

	// 是否可以激活
	virtual bool CanActive()const;
	
	// 激活
	virtual bool Active();

	//  取消 激活
	virtual void Cancel();
	
	//  取消
	virtual void End();

	// 装备至插槽
	virtual void Allocation();

	// 从插槽移除
	virtual void UnAllocation();

	IDType GetID()const;

	FGameplayTag GetUnitType()const;

	TSoftObjectPtr<UTexture2D> GetIcon()const;

	// 
	FString GetUnitName()const;

	FOnAllocationCharacterUnitChanged OnAllocationCharacterUnitChanged;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr);

	ACharacterBase* GetOwnerCharacter()const;

	ACharacterBase* GetAllocationCharacter()const;

	// 这个物品所在的对象
	TWeakPtr<FCharacterProxy> GetOwnerCharacterProxy()const;

	// 这个物品被分配给的对象
	TWeakPtr<FCharacterProxy> GetAllocationCharacterProxy();

	TWeakPtr<FCharacterProxy> GetAllocationCharacterProxy()const;

	void Update2Client();

protected:

	FTableRowUnit* GetTableRowUnit()const;

	UPROPERTY(Transient)
	FGameplayTag UnitType = FGameplayTag::EmptyTag;

	IDType ID;

	// 
	IDType OwnerCharacter_ID;

	// 被分配给的对象ID
	IDType AllocationCharacter_ID;

	// Root组件,不为空
	UHoldingItemsComponent* HoldingItemsComponentPtr = nullptr;

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

	void UpdateByRemote(const TSharedPtr<FCoinProxy>& RemoteSPtr);

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

	void UpdateByRemote(const TSharedPtr<FConsumableProxy>& RemoteSPtr);

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

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

	int32 Num = 1;

protected:

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
