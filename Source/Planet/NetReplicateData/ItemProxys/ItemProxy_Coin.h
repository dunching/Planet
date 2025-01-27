// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxy.h"

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
struct PLANET_API FCoinProxy : public FAllocationbleProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;

	FCoinProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	void UpdateByRemote(const TSharedPtr<FCoinProxy>& RemoteSPtr);

	void AddCurrentValue(int32 val);

	int32 GetCurrentValue()const;

	int32 GetOffsetNum()const;

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

protected:

	// 总数
	int32 Num = 1;

	// 这次的增量/减量
	int32 OffsetNum = 1;

};

template<>
struct TStructOpsTypeTraits<FCoinProxy> :
	public TStructOpsTypeTraitsBase2<FCoinProxy>
{
	enum
	{
		WithNetSerializer = false,
	};
};
