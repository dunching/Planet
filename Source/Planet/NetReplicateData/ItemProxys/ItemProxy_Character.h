// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "AttributeSet.h"
#include "ItemProxy.h"
#include "ItemProxy_Interface.h"

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
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;
struct FBasicProxy;

class UItemProxy_Description_Character;

using FOnCharacterSocketUpdated = TMulticastDelegate<void(
	const FCharacterSocket&,
	const FGameplayTag&


	
	)>;

/*
 * 角色的角色的武器、技能插槽分配配置
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

	/**
	 * 插槽
	 */
	FGameplayTag Socket;

private:
	/**
	 * 插槽里分配的物品ID
	 */
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

/*
 * 角色的天赋配置
 */
USTRUCT()
struct PLANET_API FCharacterTalent
{
	GENERATED_USTRUCT_BODY()
	
	bool NetSerialize(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		);

	/**
	 * 插槽、点数
	 */
	TMap<FGameplayTag, uint8> AllocationMap;
};

template <>
struct TStructOpsTypeTraits<FCharacterTalent> :
	public TStructOpsTypeTraitsBase2<FCharacterTalent>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FCharacterProxy :
	public FBasicProxy,
	public IProxy_Allocationble
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

	UItemProxy_Description_Character* GetTableRowProxy_Character() const;

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

	FString GetDisplayTitle() const;

	void AddExperience(uint32 Value);

	uint8 GetLevel()const;
	
	uint8 GetExperience()const;
	
	uint8 GetLevelExperience()const;
	
	FOnCharacterSocketUpdated OnCharacterSocketUpdated;

	TOnValueChangedCallbackContainer<uint8> LevelChangedDelegate;

	TOnValueChangedCallbackContainer<uint8> ExperienceChangedDelegate;

	TOnValueChangedCallbackContainer<uint8> LevelExperienceChangedDelegate;

protected:
	TMap<FGameplayTag, FCharacterSocket> TeammateConfigureMap;

	FCharacterTalent CharacterTalent;

	TWeakObjectPtr<FPawnType> ProxyCharacterPtr = nullptr;

private:
	// 当前等级
	uint8 Level = 1;

	// 当前经验
	int32 Experience = 0;

	FString Title;

	FString Name;
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
