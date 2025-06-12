// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"
#include "Planet_ItemProxy.h"

#include "ItemProxy_Companion.generated.h"

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

struct FAllocationSkills;
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

class APlanet_Consumable_Base;
class USkill_Consumable_Base;
class UInventoryComponent;
class UItemProxy_Description_Consumable;

UCLASS()
class PLANET_API UItemProxy_Description_Companion : public UItemProxy_Description
{
	GENERATED_BODY()

public:
};

/**
 * 伙伴道具
 * 用于伙伴类FCharacterProxy强化的道具
 * 如【射雕】里面的【侠士经略】
 */
USTRUCT()
struct PLANET_API FCompanionProxy :
	public FPlanet_BasicProxy,
	public IProxy_SkillState,
	public IProxy_Unique
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FCompanionProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		) override;

	void UpdateByRemote(
		const TSharedPtr<FCompanionProxy>& RemoteSPtr
		);

	virtual bool Active() override;

	UItemProxy_Description_Consumable* GetTableRowProxy_Consumable() const;

	virtual void UpdateData() override;

#pragma region IProxy_Unique
	virtual void ModifyNum(int32 Value) override;
#pragma endregion

#pragma region Cooldown interface
	virtual int32 GetCount() const override;

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown,
		float& RemainingCooldownPercent
		) const override;

	virtual bool CheckNotInCooldown() const override;

	virtual void AddCooldownConsumeTime(
		float NewTime
		) override;

	virtual void FreshUniqueCooldownTime() override;

	virtual void ApplyCooldown() override;

	virtual void OffsetCooldownTime() override;
#pragma endregion

protected:
};

template <>
struct TStructOpsTypeTraits<FCompanionProxy> :
	public TStructOpsTypeTraitsBase2<FCompanionProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
