// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"

#include "TemplateHelper.h"

#include "ItemProxy_Coin.generated.h"

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

USTRUCT()
struct PLANET_API FCoinProxy :
	public FBasicProxy,
	public IProxy_Allocationble,
	public IProxy_Unique
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FCoinProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(const FGameplayTag& InProxyType) override;

private:
	void UpdateByRemote(const TSharedPtr<FCoinProxy>& RemoteSPtr);

	virtual bool IsUnique() const override;

protected:
};

template <>
struct TStructOpsTypeTraits<FCoinProxy> :
	public TStructOpsTypeTraitsBase2<FCoinProxy>
{
	enum
	{
		WithNetSerializer = false,
	};
};
