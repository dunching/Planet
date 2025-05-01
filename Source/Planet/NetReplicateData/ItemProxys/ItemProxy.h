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

class UItemProxy_Description;
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
	
	/**
	 * 获取GA冷却
	 * @param RemainingCooldown 
	 * @param RemainingCooldownPercent 
	 * @return 
	 */
	virtual bool GetRemainingCooldown(
		float& RemainingCooldown, float& RemainingCooldownPercent
	) const = 0;

	virtual bool CheckNotInCooldown() const = 0;

	/**
	 * 增加或减少GA冷却
	 */
	virtual void AddCooldownConsumeTime(float NewTime) = 0;

	/**
	 * 刷新冷却
	 */
	virtual void FreshUniqueCooldownTime() = 0;

	virtual void OffsetCooldownTime() = 0;

	virtual void ApplyCooldown() = 0;
};

/**
 * 序列化&反序列化
 * 通用数据记录在DataTable，变化数据记录在对象内
 * 场景内的对象代理
 */
USTRUCT()
struct PLANET_API FBasicProxy
{
	GENERATED_USTRUCT_BODY()

public:
	using FOnAllocationCharacterProxyChanged =
	TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)>;

	friend FSceneProxyContainer;
	friend FCharacterProxy;
	friend FProxy_FASI_Container;
	friend UInventoryComponent;
	friend APlanetGameMode;

	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	// FBasicProxy operator=(const FBasicProxy&) = delete;
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual void InitialProxy(const FGameplayTag& ProxyType);

	// 通过远程的更新客户端的数据
	void UpdateByRemote(const TSharedPtr<FBasicProxy>& RemoteSPtr);

	// 是否可以激活
	virtual bool CanActive() const;

	// 激活
	virtual bool Active();

	//  取消 激活
	virtual void Cancel();

	//  取消
	virtual void End();

	virtual int32 GetNum()const;

	IDType GetID() const;

	FGameplayTag GetProxyType() const;

	TSoftObjectPtr<UTexture2D> GetIcon() const;

	// 
	FString GetProxyName() const;

	void Update2Client();

	TObjectPtr<UItemProxy_Description>GetItemProxy_Description()const;
	
	UInventoryComponent*GetInventoryComponent() const;

private:

	/**
	 * 是否是唯一的，比如货币
	 * @return 
	 */
	virtual bool IsUnique()const;

protected:
	
	// 装备至插槽
	virtual void Allocation();

	// 从插槽移除
	virtual void UnAllocation();

	FTableRowProxy* GetTableRowProxy() const;

	// Root组件,不为空
	UInventoryComponent* InventoryComponentPtr = nullptr;
	
private:
	
	UPROPERTY(Transient)
	FGameplayTag ProxyType = FGameplayTag::EmptyTag;

	IDType ID;

};

template <>
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

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	void UpdateByRemote(const TSharedPtr<FAllocationbleProxy>& RemoteSPtr);

	// 这个物品被分配给的对象
	TWeakPtr<FCharacterProxy> GetAllocationCharacterProxy();

	TWeakPtr<FCharacterProxy> GetAllocationCharacterProxy() const;

	ACharacterBase* GetAllocationCharacter() const;

	virtual void SetAllocationCharacterProxy(
		const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
		const FGameplayTag& InSocketTag
	);

	virtual void ResetAllocationCharacterProxy();

	FGameplayTag GetCurrentSocketTag() const;

	IDType GetAllocationCharacterID() const;

	FOnAllocationCharacterProxyChanged OnAllocationCharacterProxyChanged;

protected:
	// 被分配给的对象ID
	IDType AllocationCharacter_ID;

	// 所在插槽
	FGameplayTag SocketTag;
};

template <>
struct TStructOpsTypeTraits<FAllocationbleProxy> :
	public TStructOpsTypeTraitsBase2<FAllocationbleProxy>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT()
struct PLANET_API FToolProxy : public FAllocationbleProxy
{
	GENERATED_USTRUCT_BODY()

public:
	friend FSceneProxyContainer;
	friend UInventoryComponent;

	FToolProxy();

	virtual int32 GetNum() const override;

	int32 DamageDegree = 0;

protected:
	int32 Num = 1;
};
