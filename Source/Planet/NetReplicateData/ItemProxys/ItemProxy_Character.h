// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "ItemProxy.h"

#include "ItemProxy_Character.generated.h"

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
struct FBasicProxy;

using FOnCharacterSocketUpdated = TMulticastDelegate<void(
	const FCharacterSocket&,
	const FGameplayTag&

)>;

/*
 * 角色的配置
 * 比如角色的武器插槽，技能插槽分配
 */
USTRUCT()
struct PLANET_API FCharacterSocket
{
	GENERATED_USTRUCT_BODY()

	bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	);

	void UpdateProxy(
		const TSharedPtr<FBasicProxy>& ProxySPtr
	);

	void SetAllocationedProxyID(
		const FGuid& NewID
	);

	FGuid GetAllocationedProxyID() const;

	bool operator()() const;

	bool IsValid() const;

	void ResetAllocatedProxy();

	bool operator==(
		const FCharacterSocket& Other
	) const
	{
		return Socket == Other.Socket && AllocationedProxyID == Other.AllocationedProxyID;
	}

	bool operator!=(
		const FCharacterSocket& Other
	) const
	{
		return !(*this == Other);
	}

	friend inline uint32 GetTypeHash(
		const FCharacterSocket& Key
	)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Key.Socket));
		Hash = HashCombine(Hash, GetTypeHash(Key.AllocationedProxyID));
		return Hash;
	}

	FGameplayTag Socket;

private:
	// 分配的物品ID
	FGuid AllocationedProxyID;
};

template <>
struct TStructOpsTypeTraits<FCharacterSocket> :
	public TStructOpsTypeTraitsBase2<FCharacterSocket>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FCharacterProxy : public FAllocationbleProxy
{
	GENERATED_USTRUCT_BODY()

public:
	friend UInventoryComponent;

	using FPawnType = ACharacterBase;

	FCharacterProxy();

	FCharacterProxy(
		const IDType& ID
	);

	void UpdateByRemote(
		const TSharedPtr<FCharacterProxy>& RemoteSPtr
	);

	virtual void InitialProxy(
		const FGameplayTag& ProxyType
	) override;

	virtual bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
	) override;

	FTableRowProxy_CharacterGrowthAttribute* GetDT_CharacterInfo() const;

	FTableRowProxy_CharacterType* GetDT_CharacterType() const;

	// 解除这个类下AddToRoot的对象
	void RelieveRootBind();

	/**
	 * 生成一个NPC Actor
	 * @param Transform 
	 * @return 
	 */
	AHumanCharacter_AI* SpwanCharacter(
		const FTransform& Transform
	);

	void DestroyCharacter();

	// 通过插槽
	FCharacterSocket FindSocket(
		const FGameplayTag& SocketID
	) const;

	// 通过指定代理类型，
	// 比如我们需要确认是否装备了斧头
	// 比如我们需要查询我们的插槽里面是否使用了“换技能”的代理
	FCharacterSocket FindSocketByType(
		const FGameplayTag& ProxyType
	) const;

	void GetWeaponSocket(
		FCharacterSocket& FirstWeaponSocketInfoSPtr,
		FCharacterSocket& SecondWeaponSocketInfoSPtr
	);

	TMap<FGameplayTag, FCharacterSocket> GetSockets() const;;

	// 获取这个代理的CharacterActor，如果有的话
	TWeakObjectPtr<FPawnType> GetCharacterActor() const;

	void UpdateSocket(
		const FCharacterSocket& Socket
	);

	TSharedPtr<FCharacterAttributes> CharacterAttributesSPtr = nullptr;

	FString Title;

	FString Name;

	int32 Level = 1;

	FOnCharacterSocketUpdated OnCharacterSocketUpdated;

protected:
	TMap<FGameplayTag, FCharacterSocket> TeammateConfigureMap;

	TWeakObjectPtr<FPawnType> ProxyCharacterPtr = nullptr;
};

template <>
struct TStructOpsTypeTraits<FCharacterProxy> :
	public TStructOpsTypeTraitsBase2<FCharacterProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};
