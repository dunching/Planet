// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"
#include "Planet_ItemProxy.h"

#include "ItemProxy_Consumable.generated.h"

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
class PLANET_API UItemProxy_Description_Consumable : public UItemProxy_Description
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<APlanet_Consumable_Base> Consumable_Class;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<USkill_Consumable_Base> Skill_Consumable_Class;

	// UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	// TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TMap<FGameplayTag, int32> ModifyPropertyMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	UAnimMontage* HumanMontage = nullptr;

	// 消耗品公共CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FGameplayTag> CommonCooldownInfoMap;

	// 消耗品独立CD
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CD = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float Duration = 3.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	float PerformActionInterval = 1.f;
};

/**
 * 消耗品道具
 */
USTRUCT()
struct PLANET_API FConsumableProxy :
	public FPlanet_BasicProxy,
	public IProxy_Allocationble,
	public IProxy_SkillState,
	public IProxy_Unique
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FConsumableProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		) override;

	void UpdateByRemote(
		const TSharedPtr<FConsumableProxy>& RemoteSPtr
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
struct TStructOpsTypeTraits<FConsumableProxy> :
	public TStructOpsTypeTraitsBase2<FConsumableProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
