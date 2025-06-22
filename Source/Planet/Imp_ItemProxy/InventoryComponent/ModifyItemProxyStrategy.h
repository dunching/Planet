// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "ItemProxy_Material.h"

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

	virtual TArray<TSharedPtr<FBasicProxy>> FindByType(
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

	TMap<FGameplayTag, TArray<TSharedPtr<FItemProxyType>>> ProxyTypeMap;
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

	virtual TArray<TSharedPtr<FBasicProxy>> FindByType(
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

	TMap<FGameplayTag, TArray<TSharedPtr<FItemProxyType>>> ProxyTypeMap;
};

struct PLANET_API FModifyItemProxyStrategy_MaterialProxy : public FModifyItemProxyStrategyIterface
{
	using FItemProxyType = FMaterialProxy;

	using FOnProxyChanged = TCallbackHandleContainer<void(
		const TSharedPtr<
			FMaterialProxy>&,
		// 直接指定类型
		EProxyModifyType,
		int32
		)
	>;

	virtual FGameplayTag GetCanOperationType() const override;

	virtual void FindByID(
		const FGuid& ID,
		const TSharedPtr<FBasicProxy>& FindResultSPtr,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) override;

	virtual TArray<TSharedPtr<FBasicProxy>> FindByType(
		const FGameplayTag& ProxyType,
		const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
		) const override;

	virtual TArray<TSharedPtr<FBasicProxy>> Add(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		) override;

	virtual void Update(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
		) override;

	virtual void RemoveItemProxy(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGuid& InProxyID
		) override;

	virtual TSharedPtr<FBasicProxy> AddByRemote(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
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

	virtual TSharedPtr<FBasicProxy> NetSerialize(
		const FGameplayTag ProxyType,
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		) override;

	FOnProxyChanged OnProxyChanged;

	TSharedPtr<FItemProxyType> ResultSPtr = nullptr;

private:
	template <typename MaterialProxyType>
	TArray<TSharedPtr<FBasicProxy>> AddImp(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const FGameplayTag& InProxyType,
		int32 Num
		);

	template <typename MaterialProxyType>
	TSharedPtr<FBasicProxy> AddByRemoteImp(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
		);

	template <typename MaterialProxyType>
	TSharedPtr<FBasicProxy> UpdateByRemoteImp(
		const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
		const TSharedPtr<FBasicProxy>& LocalProxySPtr,
		const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
		);

	template <typename MaterialProxyType>
	TSharedPtr<FBasicProxy> NetSerializeImp(
		FArchive& Ar,
		class UPackageMap* Map,
		bool& bOutSuccess
		);

	TMap<FGameplayTag, TArray<TSharedPtr<FItemProxyType>>> ProxyTypeMap;
};

template <typename MaterialProxyType>
TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_MaterialProxy::AddImp(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto TempNum = Num;

	TArray<TSharedPtr<FBasicProxy>> Result;

	TSharedPtr<MaterialProxyType> NewResultSPtr = nullptr;

	auto ProxyAry = InventoryComponentPtr->FindAllProxyType(InProxyType);
	if (ProxyAry.IsValidIndex(0))
	{
		for (const auto& Iter : ProxyAry)
		{
			NewResultSPtr = DynamicCastSharedPtr<MaterialProxyType>(Iter);

			const auto Offset = NewResultSPtr->GetMaxNum() - NewResultSPtr->GetNum();
			if (Offset < TempNum)
			{
				NewResultSPtr->ModifyNum(Offset);
				TempNum -= Offset;
			}
			else
			{
				NewResultSPtr->ModifyNum(TempNum);

				TempNum = 0;
			}
			InventoryComponentPtr->UpdateInContainer(NewResultSPtr);

			Result.Add(NewResultSPtr);

			if (TempNum <= 0)
			{
				break;
			}
		}
	}
	else
	{
	}

	for (; TempNum > 0;)
	{
		NewResultSPtr = MakeShared<MaterialProxyType>();

		NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
		NewResultSPtr->InitialProxy(InProxyType);

		const auto Offset = NewResultSPtr->GetMaxNum();
		if (Offset < TempNum)
		{
			NewResultSPtr->ModifyNum(Offset);

			TempNum -= Offset;
		}
		else
		{
			NewResultSPtr->ModifyNum(TempNum);

			TempNum = 0;
		}

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		if (ProxyTypeMap.Contains(InProxyType))
		{
			ProxyTypeMap[InProxyType].Add(NewResultSPtr);
		}
		else
		{
			ProxyTypeMap.Add(InProxyType, {NewResultSPtr});
		}

		Result.Add(NewResultSPtr);
	}

	OnProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);

	return Result;
}

template <typename MaterialProxyType>
TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_MaterialProxy::AddByRemoteImp(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
	)
{
	auto NewResultSPtr = MakeShared<MaterialProxyType>();

	NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);

	NewResultSPtr->UpdateByRemote(DynamicCastSharedPtr<MaterialProxyType>(InRemoteProxySPtr));

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	const auto InProxyType = NewResultSPtr->GetProxyType();
	if (ProxyTypeMap.Contains(InProxyType))
	{
		ProxyTypeMap[InProxyType].Add(NewResultSPtr);
	}
	else
	{
		ProxyTypeMap.Add(InProxyType, {NewResultSPtr});
	}

	OnProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, NewResultSPtr->GetNum());

	return NewResultSPtr;
}

template <typename MaterialProxyType>
TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_MaterialProxy::UpdateByRemoteImp(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
	)
{
	auto RightProxySPtr = DynamicCastSharedPtr<MaterialProxyType>(InRemoteProxySPtr);

	auto LeftProxySPtr = DynamicCastSharedPtr<MaterialProxyType>(LocalProxySPtr);

	LeftProxySPtr->UpdateByRemote(RightProxySPtr);

	OnProxyChanged.ExcuteCallback(LeftProxySPtr, EProxyModifyType::kNumChanged, LeftProxySPtr->GetNum());

	return LeftProxySPtr;
}

template <typename MaterialProxyType>
TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_MaterialProxy::NetSerializeImp(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	auto NewResultSPtr = MakeShared<MaterialProxyType>();

	NewResultSPtr->NetSerialize(Ar, Map, bOutSuccess);

	return NewResultSPtr;
}
