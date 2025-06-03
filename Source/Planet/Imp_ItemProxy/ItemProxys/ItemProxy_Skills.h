// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "GameplayAbilitySpecHandle.h"
#include "GenerateTypes.h"

#include "ItemProxy.h"
#include "ItemProxy_Description.h"
#include "ItemProxy_Interface.h"
#include "Planet_ItemProxy.h"

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
class APlanetWeapon_Base;
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

struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;
struct FWeaponProxy;

enum struct ECharacterPropertyType : uint8;

#pragma region 技能

UCLASS()
class PLANET_API UItemProxy_Description_Skill : public UItemProxy_Description
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<USkill_Base> SkillClass;
};

USTRUCT()
struct PLANET_API FSkillProxy :
	public FPlanet_BasicProxy,
	public IProxy_Allocationble
{
	GENERATED_USTRUCT_BODY()

public:
	friend FWeaponProxy;
	friend FSceneProxyContainer;
	friend UInventoryComponent;

	FSkillProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		) override;

	virtual void SetAllocationCharacterProxy(
		const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
		const FGameplayTag& InSocketTag
		) override;

	void UpdateByRemote(
		const TSharedPtr<FSkillProxy>& RemoteSPtr
		);

	virtual TSubclassOf<USkill_Base> GetSkillClass() const;

	TArray<USkill_Base*> GetGAInstAry() const;

	USkill_Base* GetGAInst() const;

	// 注册GA
	virtual void RegisterSkill();

	// 取消注册GA
	void UnRegisterSkill();

	FGameplayAbilitySpecHandle GetGAHandle() const;

	int32 Level = 1;

protected:
	// 装备至插槽
	virtual void Allocation() override;

	// 从插槽移除
	virtual void UnAllocation() override;

	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;

private:
};

template <>
struct TStructOpsTypeTraits<FSkillProxy> :
	public TStructOpsTypeTraitsBase2<FSkillProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
#pragma endregion

#pragma region 被动技能
UCLASS()
class PLANET_API UItemProxy_Description_PassiveSkill : public UItemProxy_Description_Skill
{
	GENERATED_BODY()

public:
};

USTRUCT()
struct PLANET_API FPassiveSkillProxy :
	public FSkillProxy,
	public IProxy_SkillState
{
	GENERATED_USTRUCT_BODY()

public:
	FPassiveSkillProxy();

	virtual void InitialProxy(
		const FGameplayTag& ProxyType
		) override;

	UItemProxy_Description_PassiveSkill* GetTableRowProxy_PassiveSkillExtendInfo() const;

	FTableRowProxy_PropertyEntrys* GetMainPropertyEntry() const;

	virtual TSubclassOf<USkill_Base> GetSkillClass() const override;

#pragma region IProxy_Cooldown
	virtual int32 GetCount() const override;

	// return 是否在冷却
	virtual bool GetRemainingCooldown(
		float& RemainingCooldown,
		float& RemainingCooldownPercent
		) const override;

	// return 是否在冷却
	virtual bool CheckNotInCooldown() const override;

	virtual void AddCooldownConsumeTime(
		float CDOffset
		) override;

	virtual void FreshUniqueCooldownTime() override;

	virtual void ApplyCooldown() override;

	virtual void OffsetCooldownTime() override;
#pragma endregion

	// 元素词条
	FGameplayTag ElementPropertyEntry;

	// 主词条
	FGameplayTag MainPropertyEntry;

	// 副词条
	FGameplayTag SecondPropertyEntry;

protected:
	// 装备至插槽
	virtual void Allocation() override;

	// 从插槽移除
	virtual void UnAllocation() override;
};
#pragma endregion

#pragma region 主动技能
UCLASS()
class PLANET_API UItemProxy_Description_ActiveSkill : public UItemProxy_Description_Skill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag RequireWeaponProxyType = FGameplayTag::EmptyTag;

	// 技能公共CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FGameplayTag> SkillCommonCooldownInfoMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float CD = {60, 50, 40, 30, 20};

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FPerLevelValue_Float Cost = {60, 50, 40, 30, 20};
};

USTRUCT()
struct PLANET_API FActiveSkillProxy :
	public FSkillProxy,
	public IProxy_SkillState
{
	GENERATED_USTRUCT_BODY()

public:
	FActiveSkillProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	// 是否可以激活
	virtual bool CanActive() const override;

	virtual bool Active() override;

	virtual void Cancel() override;

	UItemProxy_Description_ActiveSkill* GetTableRowProxy_ActiveSkillExtendInfo() const;

	virtual TSubclassOf<USkill_Base> GetSkillClass() const override;

#pragma region IProxy_Cooldown
	virtual int32 GetCount() const override;

	// return 是否在冷却
	virtual bool GetRemainingCooldown(
		float& RemainingCooldown,
		float& RemainingCooldownPercent
		) const override;

	// return 是否在冷却
	virtual bool CheckNotInCooldown() const override;

	virtual void AddCooldownConsumeTime(
		float CDOffset
		) override;

	virtual void FreshUniqueCooldownTime() override;

	virtual void ApplyCooldown() override;

	virtual void OffsetCooldownTime() override;
#pragma endregion

	// void SetCDGEChandle(FActiveGameplayEffectHandle InCD_GE_Handle);

protected:
	// FActiveGameplayEffectHandle CD_GE_Handle;
};
#pragma endregion

#pragma region 元素技能
USTRUCT()
struct PLANET_API FTalentSkillProxy : public FSkillProxy
{
	GENERATED_USTRUCT_BODY()

public:
	FTalentSkillProxy();

protected:
};
#pragma endregion

#pragma region 武器技能
UCLASS()
class PLANET_API UItemProxy_Description_WeaponSkill : public UItemProxy_Description_Skill
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	EElementalType ElementalType = EElementalType::kMetal;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	int32 Elemental_Damage = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	float Elemental_Damage_Magnification = .5f;
};

USTRUCT()
struct PLANET_API FWeaponSkillProxy : public FSkillProxy
{
	GENERATED_USTRUCT_BODY()

public:
	friend FCharacterProxy;

	FWeaponSkillProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual bool Active() override;

	virtual void Cancel() override;

	virtual void End() override;

	UItemProxy_Description_WeaponSkill* GetTableRowProxy_WeaponSkillExtendInfo() const;

	virtual TSubclassOf<USkill_Base> GetSkillClass() const override;

	TObjectPtr<APlanetWeapon_Base> ActivedWeaponPtr = nullptr;

protected:
	virtual void RegisterSkill() override;
};

template <>
struct TStructOpsTypeTraits<FWeaponSkillProxy> :
	public TStructOpsTypeTraitsBase2<FWeaponSkillProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

#pragma endregion
