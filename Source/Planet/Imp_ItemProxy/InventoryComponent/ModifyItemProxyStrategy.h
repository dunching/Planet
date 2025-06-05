// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "ModifyItemProxyStrategyInterface.h"
#include "TemplateHelper.h"
#include "ItemProxy_Minimal.h"

struct FCoinProxy;
struct FConsumableProxy;
struct FCharacterProxy;
struct FWeaponProxy;
struct FSkillProxy;
struct FPassiveSkillProxy;
struct FActiveSkillProxy;
struct FWeaponSkillProxy;

struct PLANET_API FModifyItemProxyStrategy_Character : public FModifyItemProxyStrategyBase<FCharacterProxy>
{
	using FItemProxyType = FCharacterProxy;

	using FOnGroupmateProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FItemProxyType>&,
		EProxyModifyType
		)
	>;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	FOnGroupmateProxyChanged OnGroupmateProxyChanged;
};

struct PLANET_API FModifyItemProxyStrategy_Weapon : public FModifyItemProxyStrategyBase<FWeaponProxy>
{
	using FItemProxyType = FWeaponProxy;

	using FOnWeaponProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FItemProxyType>&,
		EProxyModifyType
		)
	>;
	virtual FGameplayTag GetCanOperationType() const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	FOnWeaponProxyChanged OnWeaponProxyChanged;
};

using FModifyItemProxyStrategy_Skill = FModifyItemProxyStrategyBase<
	FSkillProxy>;

struct PLANET_API IModifyItemProxyStrategy_Skill_Interface
{
	using FItemProxyType = FSkillProxy;

	using FOnSkillProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FItemProxyType>&,
		EProxyModifyType
		)>;

	FOnSkillProxyChanged OnSkillProxyChanged;
};

struct PLANET_API FModifyItemProxyStrategy_WeaponSkill :
	public IModifyItemProxyStrategy_Skill_Interface,
	public FModifyItemProxyStrategyBase<
		FWeaponSkillProxy>
{
	using FItemProxyType = FWeaponSkillProxy;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;
};

struct PLANET_API FModifyItemProxyStrategy_ActiveSkill :
	public IModifyItemProxyStrategy_Skill_Interface,
	public FModifyItemProxyStrategyBase<
		FActiveSkillProxy>
{
	using FItemProxyType = FActiveSkillProxy;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;
};

struct PLANET_API FModifyItemProxyStrategy_PassveSkill :
	public IModifyItemProxyStrategy_Skill_Interface,
	public FModifyItemProxyStrategyBase<
		FPassiveSkillProxy>
{
	using FItemProxyType = FPassiveSkillProxy;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;
};

struct PLANET_API FModifyItemProxyStrategy_Coin : public FModifyItemProxyStrategyBase<FCoinProxy>
{
	using FItemProxyType = FCoinProxy;

	using FOnCoinProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FItemProxyType>&,
		EProxyModifyType,
		int32
		)>;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual TSharedPtr<FBasicProxy> FindByType(
		const FGameplayTag& ProxyType,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual TSharedPtr<FBasicProxy> UpdateByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual void RemoveByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	FOnCoinProxyChanged OnCoinProxyChanged;

	TMap<FGameplayTag, TSharedPtr<FItemProxyType>> ProxyTypeMap;
};

struct PLANET_API FModifyItemProxyStrategy_Consumable : public FModifyItemProxyStrategyBase<FConsumableProxy>
{
	using FItemProxyType = FConsumableProxy;

	using FOnConsumableProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FItemProxyType>&,
		EProxyModifyType,
		int32
		)
	>;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual TSharedPtr<FBasicProxy> FindByType(
		const FGameplayTag& ProxyType,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual TSharedPtr<FBasicProxy> UpdateByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	virtual void RemoveByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& RemoteProxySPtr
		) override;

	FOnConsumableProxyChanged OnConsumableProxyChanged;

	TMap<FGameplayTag, TSharedPtr<FItemProxyType>> ProxyTypeMap;
};
