#include "ModifyItemProxyStrategy.h"

#include "GameplayTagsLibrary.h"
#include "InventoryComponentBase.h"
#include "ItemProxy_Character.h"
#include "ItemProxy_Skills.h"
#include "ItemProxy_Consumable.h"
#include "ItemProxy_Coin.h"
#include "ItemProxy_Weapon.h"

FGameplayTag FModifyItemProxyStrategy_Character::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Character;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_Character::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
	NewResultSPtr->InitialProxy(InProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	return {NewResultSPtr};
}

FGameplayTag FModifyItemProxyStrategy_Weapon::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Weapon;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_Weapon::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
	NewResultSPtr->InitialProxy(InProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	NewResultSPtr->WeaponSkillID =
		InventoryComponentPtr->AddProxy(
		                                NewResultSPtr->GetTableRowProxy_WeaponExtendInfo()->WeaponSkillProxyType
		                               )->GetID();

	OnWeaponProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);

	return {NewResultSPtr};
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Weapon::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<FItemProxyType>::AddByRemote(
		                                                           InventoryComponentPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	OnWeaponProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged);

	return NewResultSPtr;
}

FGameplayTag FModifyItemProxyStrategy_WeaponSkill::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Skill_Weapon;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_WeaponSkill::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
	NewResultSPtr->InitialProxy(InProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);

	return {NewResultSPtr};
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_WeaponSkill::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<FItemProxyType>::AddByRemote(
		                                                           InventoryComponentPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	OnSkillProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged);

	return NewResultSPtr;
}

FGameplayTag FModifyItemProxyStrategy_ActiveSkill::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Skill_Active;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_ActiveSkill::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
	NewResultSPtr->InitialProxy(InProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);

	return {NewResultSPtr};
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_ActiveSkill::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<FItemProxyType>::AddByRemote(
		                                                           InventoryComponentPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	OnSkillProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged);

	return NewResultSPtr;
}

FGameplayTag FModifyItemProxyStrategy_PassveSkill::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Skill_Passve;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_PassveSkill::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
	NewResultSPtr->InitialProxy(InProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);

	return {NewResultSPtr};
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_PassveSkill::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<FItemProxyType>::AddByRemote(
		                                                           InventoryComponentPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	OnSkillProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged);

	return NewResultSPtr;
}

FGameplayTag FModifyItemProxyStrategy_Coin::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Coin;
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Coin::FindByType(
	const FGameplayTag& ProxyType,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	) const
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		return ProxyTypeMap[ProxyType];
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_Coin::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxyType(InProxyType))
	{
		auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(ProxySPtr);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->UpdateInContainer(NewResultSPtr);

		OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);

		return {NewResultSPtr};
	}
	else
	{
		auto NewResultSPtr = MakeShared<FItemProxyType>();

		NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
		NewResultSPtr->InitialProxy(InProxyType);

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		NewResultSPtr->ModifyNum(Num);

		ProxyTypeMap.Add(InProxyType, NewResultSPtr);

		OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);

		return {NewResultSPtr};
	}
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Coin::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<FItemProxyType>::AddByRemote(
		                                                           InventoryComponentPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	ProxyTypeMap.Add(NewResultSPtr->GetProxyType(), NewResultSPtr);

	OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, NewResultSPtr->GetNum());

	return NewResultSPtr;
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Coin::UpdateByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<
		                                                          FItemProxyType>::UpdateByRemote(
		                                                           InventoryComponentPtr,
		                                                           LocalProxySPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, NewResultSPtr->GetNum());

	return NewResultSPtr;
}

void FModifyItemProxyStrategy_Coin::RemoveByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	FModifyItemProxyStrategyBase<
		FItemProxyType>::RemoveByRemote(
										InventoryComponentPtr,
										RemoteProxySPtr
									   );

	ProxyTypeMap.Remove(RemoteProxySPtr->GetProxyType());
	
	OnCoinProxyChanged.ExcuteCallback(
											DynamicCastSharedPtr<FItemProxyType>(RemoteProxySPtr),
											EProxyModifyType::kRemove,
											0
										   );
}

FGameplayTag FModifyItemProxyStrategy_Consumable::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Consumables;
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Consumable::FindByType(
	const FGameplayTag& ProxyType,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	) const
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		return ProxyTypeMap[ProxyType];
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_Consumable::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxyType(InProxyType))
	{
		auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(ProxySPtr);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->UpdateInContainer(NewResultSPtr);

		OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);

		return {NewResultSPtr};
	}
	else
	{
		if (Num <= 0)
		{
			return {};
		}

		auto NewResultSPtr = MakeShared<FItemProxyType>();

		NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
		NewResultSPtr->InitialProxy(InProxyType);

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		NewResultSPtr->ModifyNum(Num);

		ProxyTypeMap.Add(InProxyType, NewResultSPtr);

		OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);

		return {NewResultSPtr};
	}
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Consumable::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<FItemProxyType>::AddByRemote(
		                                                           InventoryComponentPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	ProxyTypeMap.Add(NewResultSPtr->GetProxyType(), NewResultSPtr);

	OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, NewResultSPtr->GetNum());

	return NewResultSPtr;
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_Consumable::UpdateByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(
	                                                          FModifyItemProxyStrategyBase<
		                                                          FItemProxyType>::UpdateByRemote(
		                                                           InventoryComponentPtr,
		                                                           LocalProxySPtr,
		                                                           RemoteProxySPtr
		                                                          )
	                                                         );

	OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, NewResultSPtr->GetNum());

	return NewResultSPtr;
}

void FModifyItemProxyStrategy_Consumable::RemoveByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	FModifyItemProxyStrategyBase<
		FItemProxyType>::RemoveByRemote(
		                                InventoryComponentPtr,
		                                RemoteProxySPtr
		                               );

	ProxyTypeMap.Remove(RemoteProxySPtr->GetProxyType());
	
	OnConsumableProxyChanged.ExcuteCallback(
	                                        DynamicCastSharedPtr<FItemProxyType>(RemoteProxySPtr),
	                                        EProxyModifyType::kRemove,
	                                        0
	                                       );
}