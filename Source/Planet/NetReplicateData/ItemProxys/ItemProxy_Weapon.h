// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy.h"

#include "ItemProxy_Weapon.generated.h"

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
struct FCharacterProxy;
struct FWeaponSkillProxy;

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneUnitContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

enum struct ECharacterPropertyType : uint8;

USTRUCT()
struct PLANET_API FWeaponProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneUnitContainer;
	friend UHoldingItemsComponent;

	FWeaponProxy();

	void UpdateByRemote(const TSharedPtr<FWeaponProxy>& RemoteSPtr);

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
	TSharedPtr<FWeaponSkillProxy>GetWeaponSkill();

protected:

	UPROPERTY(Transient)
	int32 MaxAttackDistance = 100;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	FGuid WeaponSkillID;

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
