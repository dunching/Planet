// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"
#include "ItemProxy_Interface.h"
#include "Planet_ItemProxy.h"

#include "ItemProxy_Quests.generated.h"

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
class PLANET_API UItemProxy_Description_Quests : public UItemProxy_Description
{
	GENERATED_BODY()

public:
};

/**
 * 任务道具
 */
USTRUCT()
struct PLANET_API FQuestsProxy :
	public FPlanet_BasicProxy,
	public IProxy_Unique
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FQuestsProxy();

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	virtual void InitialProxy(
		const FGameplayTag& InProxyType
		) override;

	void UpdateByRemote(
		const TSharedPtr<FQuestsProxy>& RemoteSPtr
		);

	virtual bool Active() override;

	UItemProxy_Description_Consumable* GetTableRowProxy_Consumable() const;

	virtual void UpdateData() override;

#pragma region IProxy_Unique
	virtual void ModifyNum(int32 Value) override;
#pragma endregion

protected:
};

template <>
struct TStructOpsTypeTraits<FQuestsProxy> :
	public TStructOpsTypeTraitsBase2<FQuestsProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
