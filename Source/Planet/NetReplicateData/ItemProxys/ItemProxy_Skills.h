// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy.h"
#include "ItemProxy_Description.h"

#include "ItemProxy_Skills.generated.h"

struct FGameplayAbilityTargetData_RegisterParam;
struct FTableRowProxy_CommonCooldownInfo;
struct FTableRowProxy;
struct FTableRowProxy_WeaponExtendInfo;
struct FTableRowProxy_CharacterGrowthAttribute;
struct FTableRowProxy_Consumable;
struct FTableRowProxy_PropertyEntrys;
struct FTableRowProxy_CharacterType;
class UTexture2D;
class AToolProxyBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class USkill_Consumable_Generic;
class AConsumable_Base;
class IPlanetControllerInterface;
class APlanetPlayerState;
class APlanetGameMode;

class UItemProxy_Description_PassiveSkill;
class UItemProxy_Description_ActiveSkill;
class UItemProxy_Description_WeaponSkill;
class USkill_Base;
class ACharacterBase;
class AHumanCharacter_AI;
class AHumanCharacter;
class UInventoryComponent;
struct FCharacterProxy;

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;
struct FWeaponProxy;

enum struct ECharacterPropertyType : uint8;

USTRUCT()
struct PLANET_API FSkillProxy : public FAllocationbleProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FWeaponProxy;
	friend FSceneProxyContainer;
	friend UInventoryComponent;

	FSkillProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual void SetAllocationCharacterProxy(const TSharedPtr < FCharacterProxy>& InAllocationCharacterProxyPtr, const FGameplayTag& InSocketTag)override;

	void UpdateByRemote(const TSharedPtr<FSkillProxy>& RemoteSPtr);

	virtual TSubclassOf<USkill_Base> GetSkillClass()const;

	TArray<USkill_Base*> GetGAInstAry()const;

	USkill_Base* GetGAInst()const;

	FGameplayAbilitySpecHandle GetGAHandle()const;

	int32 Level = 1;

protected:

	// 装备至插槽
	virtual void Allocation()override;

	// 从插槽移除
	virtual void UnAllocation()override;

	virtual void RegisterSkill();

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

	virtual void InitialProxy(const FGameplayTag& ProxyType)override;

	UItemProxy_Description_PassiveSkill* GetTableRowProxy_PassiveSkillExtendInfo()const;

	FTableRowProxy_PropertyEntrys* GetMainPropertyEntry()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

	// 元素词条
	FGameplayTag ElementPropertyEntry;

	// 主词条
	FGameplayTag MainPropertyEntry;

	// 副词条
	FGameplayTag SecondPropertyEntry;

protected:

	// 装备至插槽
	virtual void Allocation()override;

	// 从插槽移除
	virtual void UnAllocation()override;

};

USTRUCT()
struct PLANET_API FActiveSkillProxy :
	public FSkillProxy,
	public IProxy_Cooldown
{
	GENERATED_USTRUCT_BODY()

public:

	FActiveSkillProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	// 是否可以激活
	virtual bool CanActive()const;

	virtual bool Active()override;

	virtual void Cancel()override;

	UItemProxy_Description_ActiveSkill* GetTableRowProxy_ActiveSkillExtendInfo()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

	// return 是否在冷却
	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const override;

	// return 是否在冷却
	virtual bool CheckCooldown()const override;

	virtual void AddCooldownConsumeTime(float NewTime)override;

	virtual void FreshUniqueCooldownTime()override;

	virtual void ApplyCooldown()override;

	virtual void OffsetCooldownTime()override;

	// void SetCDGEChandle(FActiveGameplayEffectHandle InCD_GE_Handle);
	
protected:

	// FActiveGameplayEffectHandle CD_GE_Handle;
	
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

	friend FCharacterProxy;
	
	FWeaponSkillProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual void SetAllocationCharacterProxy(const TSharedPtr < FCharacterProxy>& InAllocationCharacterProxyPtr, const FGameplayTag& InSocketTag)override;

	virtual bool Active()override;

	virtual void Cancel()override;

	virtual void End()override;

	UItemProxy_Description_WeaponSkill* GetTableRowProxy_WeaponSkillExtendInfo()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

protected:

	virtual void RegisterSkill()override;

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
