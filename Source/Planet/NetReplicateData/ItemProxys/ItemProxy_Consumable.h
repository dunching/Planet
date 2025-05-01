// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"
#include "TemplateHelper.h"

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
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

class UInventoryComponent;

USTRUCT()
struct PLANET_API FConsumableProxy :
	public FAllocationbleProxy,
	public IProxy_Cooldown
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FConsumableProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	void UpdateByRemote(const TSharedPtr<FConsumableProxy>& RemoteSPtr);

	virtual bool Active()override;

	FTableRowProxy_Consumable* GetTableRowProxy_Consumable()const;

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const override;

	virtual bool CheckNotInCooldown()const override;

	virtual void AddCooldownConsumeTime(float NewTime)override;

	virtual void FreshUniqueCooldownTime()override;

	virtual void ApplyCooldown()override;

	virtual void OffsetCooldownTime()override;

	void AddCurrentValue(int32 val);

	virtual int32 GetNum()const override;

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
