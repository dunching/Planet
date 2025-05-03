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
struct IProxy_Allocationble;

enum struct ECharacterPropertyType : uint8;

FTableRowProxy_CommonCooldownInfo* GetTableRowProxy_CommonCooldownInfo(const FGameplayTag& CommonCooldownTag);

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
	friend IProxy_Allocationble;

	using IDType = FGuid;

	FBasicProxy();

	virtual ~FBasicProxy();

	// FBasicProxy operator=(const FBasicProxy&) = delete;
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual void InitialProxy(const FGameplayTag& InProxyType);

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
