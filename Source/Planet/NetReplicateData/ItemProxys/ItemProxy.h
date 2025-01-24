// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"

#include "TemplateHelper.h"

#include "ItemProxy.generated.h"

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
class UTexture2D;
class AToolProxyBase;
class AWeapon_Base;
class USkill_Consumable_Base;
class USkill_Consumable_Generic;
class AConsumable_Base;
class IPlanetControllerInterface;
class APlanetPlayerState;
class APlanetGameMode;

class USkill_Base;
class ACharacterBase;
class AHumanCharacter_AI;
class AHumanCharacter;
class UInventoryComponent;
struct FCharacterProxy;

struct FAllocationSkills;
struct FCharacterAttributes;
struct FTalentHelper;
struct FSceneProxyContainer;
struct FProxy_FASI_Container;
struct FSkillCooldownHelper;

enum struct ECharacterPropertyType : uint8;

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag);

struct PLANET_API IProxy_Cooldown
{
public:

	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	)const = 0;

	virtual bool CheckCooldown()const = 0;

	virtual void AddCooldownConsumeTime(float NewTime) = 0;

	virtual void FreshUniqueCooldownTime() = 0;

	virtual void OffsetCooldownTime() = 0;

	virtual void ApplyCooldown() = 0;

};

// 场景内的对象代理
// 通用数据记录在DataTable，变化数据记录在对象内
// 序列化&反序列化
USTRUCT()
struct PLANET_API FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	using FOnAllocationCharacterProxyChanged =
		TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)>;

	friend FSceneProxyContainer;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;
	friend APlanetGameMode;

	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual void InitialProxy(const FGameplayTag& ProxyType);

	// 通过远程的更新客户端的数据
	void UpdateByRemote(const TSharedPtr<FBasicProxy>& RemoteSPtr);

	// 是否可以激活
	virtual bool CanActive()const;
	
	// 激活
	virtual bool Active();

	//  取消 激活
	virtual void Cancel();
	
	//  取消
	virtual void End();

	// 装备至插槽
	virtual void Allocation();

	// 从插槽移除
	virtual void UnAllocation();

	IDType GetID()const;

	FGameplayTag GetProxyType()const;

	TSoftObjectPtr<UTexture2D> GetIcon()const;

	// 
	FString GetProxyName()const;

	FOnAllocationCharacterProxyChanged OnAllocationCharacterProxyChanged;

	ACharacterBase* GetOwnerCharacter()const;

	// 这个物品所在的对象
	TWeakPtr<FCharacterProxy> GetOwnerCharacterProxy()const;

	void Update2Client();

protected:

	FTableRowProxy* GetTableRowProxy()const;

	UPROPERTY(Transient)
	FGameplayTag ProxyType = FGameplayTag::EmptyTag;

	IDType ID;

	// 
	IDType OwnerCharacter_ID;

	// Root组件,不为空
	UInventoryComponent* HoldingItemsComponentPtr = nullptr;

private:

};

template<>
struct TStructOpsTypeTraits<FBasicProxy> :
	public TStructOpsTypeTraitsBase2<FBasicProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/*
 *	可被分配至插槽的物品 
 */
USTRUCT()
struct PLANET_API FAllocationbleProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	FAllocationbleProxy();

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)override;

	void UpdateByRemote(const TSharedPtr<FAllocationbleProxy>& RemoteSPtr);

	// 这个物品被分配给的对象
	TWeakPtr<FCharacterProxy> GetAllocationCharacterProxy();

	TWeakPtr<FCharacterProxy> GetAllocationCharacterProxy()const;

	ACharacterBase* GetAllocationCharacter()const;

	virtual void SetAllocationCharacterProxy(
		const TSharedPtr < FCharacterProxy>& InAllocationCharacterProxyPtr,
		const FGameplayTag& InSocketTag
		);

	FGameplayTag GetCurrentSocketTag()const;

	IDType GetAllocationCharacterID()const;
	
protected:

	// 被分配给的对象ID
	IDType AllocationCharacter_ID;

	// 所在插槽
	FGameplayTag SocketTag;
};

template<>
struct TStructOpsTypeTraits<FAllocationbleProxy> :
	public TStructOpsTypeTraitsBase2<FAllocationbleProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FToolProxy : public FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:

	friend FSceneProxyContainer;
	friend UInventoryComponent;

	FToolProxy();

	int32 GetNum()const;

	int32 DamageDegree = 0;

protected:

	int32 Num = 1;

};
