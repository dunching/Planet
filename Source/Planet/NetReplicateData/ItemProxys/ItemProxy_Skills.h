// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy.h"

#include "ItemProxy_Skills.generated.h"

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

USTRUCT()
struct PLANET_API FSkillProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend UHoldingItemsComponent;

	FSkillProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& InAllocationCharacterUnitPtr)override;

	void UpdateByRemote(const TSharedPtr<FSkillProxy>& RemoteSPtr);

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

	// 是否可以激活
	virtual bool CanActive()const;

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

	virtual void End()override;

	FTableRowUnit_WeaponSkillExtendInfo* GetTableRowUnit_WeaponSkillExtendInfo()const;

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
