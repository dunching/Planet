// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <variant>

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include "GameplayAbilitySpecHandle.h"
#include "ItemProxy.h"

#include "TemplateHelper.h"

class ACharacterBase;

struct FBasicProxy;
struct FCharacterProxy;

enum struct ECharacterPropertyType : uint8;

/**
 * 技能状态
 * 如冷却、层数
 */
struct PLANET_ITEMPROXY_API IProxy_SkillState
{
public:
	
	virtual ~IProxy_SkillState();
	
	/**
	 * 获取层数
	 */
	virtual int32 GetCount() const = 0;

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
 * 可堆叠物品接口
 */
struct PLANET_ITEMPROXY_API IProxy_Unique
{
public:

	virtual ~IProxy_Unique();
	
	 bool NetSerialize_Unique(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) ;

	void UpdateByRemote_Unique(const TSharedPtr<IProxy_Unique>& RemoteSPtr);

	virtual void ModifyNum(int32 Value);

	int32 GetNum()const;

	int32 GetOffsetNum()const;

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

protected:
	
private:
	// 总数
	int32 Num = 0;

	// 这次的增量/减量
	int32 OffsetNum = 0;
};

PLANET_ITEMPROXY_API int32 GetProxyNum(const TSharedPtr<FBasicProxy>& ProxySPtr); 

/**
 * 可被分配的接口
 */
struct PLANET_ITEMPROXY_API IProxy_Allocationble
{
private:
	
public:

	using FOnAllocationCharacterProxyChanged =
	TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)>;

	virtual ~IProxy_Allocationble();

	bool NetSerialize_Allocationble(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	void UpdateByRemote_Allocationble(const TSharedPtr<IProxy_Allocationble>& RemoteSPtr);

	// 这个物品被分配给的对象
	TSharedPtr<FCharacterProxy> GetAllocationCharacterProxy();

	TSharedPtr<FCharacterProxy> GetAllocationCharacterProxy() const;

	ACharacterBase* GetAllocationCharacter() const;

	virtual void SetAllocationCharacterProxy(
		const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
		const FGameplayTag& InSocketTag
	);

	virtual void ResetAllocationCharacterProxy();

	FGameplayTag GetCurrentSocketTag() const;

	FGuid GetAllocationCharacterID() const;

	void SetCurrentSocketTag(const FGameplayTag&Socket);

	void SetAllocationCharacterID(const FGuid& ID);

	FOnAllocationCharacterProxyChanged OnAllocationCharacterProxyChanged;

protected:
	FBasicProxy*ProxyPtr = nullptr;
private:
	// 被分配给的对象ID
	FGuid AllocationCharacter_ID;

	// 所在插槽
	FGameplayTag SocketTag;
	
};