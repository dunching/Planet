// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"

#include "ItemProxy_Weapon.generated.h"

struct FGameplayAbilityTargetData_RegisterParam;
struct FTableRowProxy_CommonCooldownInfo;
struct FTableRowProxy;
struct FTableRowProxy_WeaponExtendInfo;
struct FTableRowProxy_ActiveSkillExtendInfo;
struct FTableRowProxy_PassiveSkillExtendInfo;
struct FTableRowProxy_WeaponSkillExtendInfo;
struct FTableRowProxy_CharacterGrowthAttribute;
struct FTableRowProxy_Consumable;
struct FTableRowProxy_PropertyEntrys;
struct FTableRowProxy_CharacterType;
struct FCharacterProxy;
struct FWeaponSkillProxy;

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

class UItemProxy_Description_Weapon;

enum struct ECharacterPropertyType : uint8;

USTRUCT()
struct PLANET_API FWeaponProxy :
	public FBasicProxy,
	public IProxy_Allocationble
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend FCharacterProxy;
	friend UInventoryComponent;

	FWeaponProxy();

	void UpdateByRemote(
		const TSharedPtr<FWeaponProxy>& RemoteSPtr
		);

	virtual void InitialProxy(
		const FGameplayTag& ProxyType
		) override;

	virtual bool Active() override;

	virtual void Cancel() override;

	virtual void SetAllocationCharacterProxy(
		const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
		const FGameplayTag& InSocketTag
		) override;

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	UItemProxy_Description_Weapon* GetTableRowProxy_WeaponExtendInfo() const;

	// 切换至当前武器
	void ActiveWeapon();

	// 收回武器
	void RetractputWeapon();

	// 主词条
	FTableRowProxy_PropertyEntrys* GetMainPropertyEntry() const;

	int32 GetMaxAttackDistance() const;

	// 注意：因为不能确定 “复制顺序”，所以这里不能用 WeakPtr
	TSharedPtr<FWeaponSkillProxy> GetWeaponSkill();

protected:
	// 装备至插槽
	virtual void Allocation() override;

	// 从插槽移除
	virtual void UnAllocation() override;

	UPROPERTY(Transient)
	int32 MaxAttackDistance = 100;

	AWeapon_Base* ActivedWeaponPtr = nullptr;

	FGuid WeaponSkillID;
};

template <>
struct TStructOpsTypeTraits<FWeaponProxy> :
	public TStructOpsTypeTraitsBase2<FWeaponProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
