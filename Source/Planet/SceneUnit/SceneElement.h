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

struct FGameplayAbilityTargetData_Skill;
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

	virtual void ApplyCooldown() = 0;

	virtual void OffsetCooldownTime() = 0;

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
	friend UHoldingItemsComponent;
	friend APlanetGameMode;

	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	virtual void InitialUnit();

	IDType GetID()const;

	FGameplayTag GetUnitType()const;

	TSoftObjectPtr<UTexture2D> GetIcon()const;

	// 
	FString GetUnitName()const;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& AllocationCharacterUnitPtr);

	TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)> OnAllocationCharacterUnitChanged;

	TWeakPtr<FCharacterProxy> GetAllocationCharacterUnit()const;

protected:

	FTableRowUnit* GetTableRowUnit()const;

	UPROPERTY(Transient)
	FGameplayTag UnitType = FGameplayTag::EmptyTag;

	// 这个物品被分配给的对象
	TWeakPtr<FCharacterProxy> AllocationCharacterUnitPtr = nullptr;
	
	// 这个物品所在的对象
	TWeakPtr<FCharacterProxy> OwnerCharacterUnitPtr = nullptr;

private:

	IDType ID;

};

USTRUCT()
struct PLANET_API FCoinProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;

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

	FConsumableProxy();

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;

	FTableRowUnit_Consumable* GetTableRowUnit_Consumable()const;

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const override;

	virtual bool CheckCooldown()const override;

	virtual void AddCooldownConsumeTime(float NewTime)override;

	virtual void FreshUniqueCooldownTime()override;

	virtual void ApplyCooldown()override;

	virtual void OffsetCooldownTime()override;

protected:

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

	int32 Num = 1;

};

USTRUCT()
struct PLANET_API FToolProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;

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

	FTableRowUnit_CharacterInfo* GetTableRowUnit_CharacterInfo()const;

	// 解除这个类下AddToRoot的对象
	void RelieveRootBind();

	TWeakObjectPtr<FPawnType> ProxyCharacterPtr = nullptr;

	TSharedPtr<FCharacterAttributes> CharacterAttributesSPtr;

	TSharedPtr<FAllocationSkills> AllocationSkills;

	TSharedPtr<FSceneUnitContainer> SceneUnitContainer;

protected:

};

USTRUCT()
struct PLANET_API FSkillProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;

	FSkillProxy();

	int32 Level = 1;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& AllocationCharacterUnitPtr)override;

	void RegisterSkill();

	void UnRegisterSkill();

	USkill_Base* GetGAInst()const;

	FGameplayAbilitySpecHandle GetGAHandle()const;

protected:

	FGameplayAbilitySpecHandle GameplayAbilitySpecHandle;

};

USTRUCT()
struct PLANET_API FPassiveSkillProxy : public FSkillProxy
{
	GENERATED_USTRUCT_BODY()

public:

	FPassiveSkillProxy();

	virtual void InitialUnit()override;

	FTableRowUnit_PassiveSkillExtendInfo* GetTableRowUnit_PassiveSkillExtendInfo()const;
	
	FTableRowUnit_PropertyEntrys*GetMainPropertyEntry()const;
	
	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

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

	FTableRowUnit_WeaponSkillExtendInfo* GetTableRowUnit_WeaponSkillExtendInfo()const;

	virtual TSubclassOf<USkill_Base> GetSkillClass()const override;

protected:

};

USTRUCT()
struct PLANET_API FWeaponProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;

	FWeaponProxy();

	virtual void InitialUnit()override;

	virtual void SetAllocationCharacterUnit(const TSharedPtr < FCharacterProxy>& AllocationCharacterUnitPtr)override;

	FTableRowUnit_WeaponExtendInfo* GetTableRowUnit_WeaponExtendInfo()const;

	// 主词条
	FTableRowUnit_PropertyEntrys* GetMainPropertyEntry()const;

	int32 GetMaxAttackDistance()const;

	TSharedPtr<FSkillProxy >FirstSkill;
	
protected:
	
	UPROPERTY(Transient)
	int32 MaxAttackDistance = 100;
	
};