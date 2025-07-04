// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"
#include "Planet_ItemProxy.h"

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
struct FTableRowProxy_GeneratiblePropertyEntrys;
struct FTableRowProxy_CharacterType;
struct FCharacterProxy;

struct FAllocationSkills;

struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

UCLASS()
class PLANET_API UItemProxy_Description_Coin : public UItemProxy_Description
{
	GENERATED_BODY()

public:
};

/**
 * 货币道具
 */
USTRUCT()
struct PLANET_API FCoinProxy :
	public FPlanet_BasicProxy,
	public IProxy_Allocationble,
	public IProxy_Unique
{
	GENERATED_USTRUCT_BODY()

public:
	FCoinProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		) override;

	void UpdateByRemote(
		const TSharedPtr<FCoinProxy>& RemoteSPtr
		);

#pragma region IProxy_Unique
	virtual void ModifyNum(int32 Value) override;
#pragma endregion

private:
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
