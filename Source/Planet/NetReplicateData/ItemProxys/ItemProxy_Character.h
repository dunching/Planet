// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"

#include "ItemProxy_Character.generated.h"

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

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneUnitContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;
struct FBasicProxy;

/*
 * 角色的配置
 * 比如角色的武器插槽，技能插槽分配
 */ 
USTRUCT()
struct PLANET_API FCharacterSocket 
{
	GENERATED_USTRUCT_BODY()
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	void UpdateProxy(const TSharedPtr<FBasicProxy>&ProxySPtr);

	bool operator()() const;
	
	bool IsValid() const;
	
	void ResetSocket();
	
	FGameplayTag Socket;

	FGuid SkillProxyID;
	
};

template<>
struct TStructOpsTypeTraits<FCharacterSocket> :
	public TStructOpsTypeTraitsBase2<FCharacterSocket>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FCharacterProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend UHoldingItemsComponent;
	
	using FPawnType = ACharacterBase;

	FCharacterProxy();

	FCharacterProxy(const IDType&ID);

	void UpdateByRemote(const TSharedPtr<FCharacterProxy>& RemoteSPtr);

	virtual void InitialUnit()override;

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	FTableRowUnit_CharacterGrowthAttribute* GetDT_CharacterInfo()const;
	
	FTableRowUnit_CharacterType* GetDT_CharacterType()const;

	// 解除这个类下AddToRoot的对象
	void RelieveRootBind();

	AHumanCharacter_AI * SpwanCharacter(const FTransform&Transform);

	void DestroyCharacter();

	// 通过插槽
	FCharacterSocket FindSocket(const FGameplayTag&SocketID)const;

	// 通过指定代理类型，比如我们需要查询我们的插槽里面是否使用了“换技能”的代理
	FCharacterSocket FindSocketByType(const FGameplayTag&ProxyType)const;

	void GetWeaponSocket(
		FCharacterSocket& FirstWeaponSocketInfoSPtr,
		FCharacterSocket& SecondWeaponSocketInfoSPtr
	);

	TWeakObjectPtr<FPawnType> ProxyCharacterPtr = nullptr;

	TSharedPtr<FCharacterAttributes> CharacterAttributesSPtr = nullptr;

	TMap<FGameplayTag, FCharacterSocket>TeammateConfigureMap;
	
	FString Title;

	FString Name;

	int32 Level = 1;

protected:

	void UpdateSocket(const FCharacterSocket&Socket);
	
};

template<>
struct TStructOpsTypeTraits<FCharacterProxy> :
	public TStructOpsTypeTraitsBase2<FCharacterProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
