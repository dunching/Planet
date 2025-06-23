#include "ModifyItemProxyStrategy.h"

#include "GameplayTagsLibrary.h"
#include "InventoryComponentBase.h"
#include "ItemProxy_Character.h"
#include "ItemProxy_Skills.h"
#include "ItemProxy_Consumable.h"
#include "ItemProxy_Coin.h"
#include "ItemProxy_Weapon.h"
#include "ItemProxy_Material.h"

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

	OnWeaponProxyChanged(NewResultSPtr, EProxyModifyType::kAdd);

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

	OnWeaponProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange);

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

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kAdd);

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

	OnSkillProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange);

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

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kAdd);

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

	OnSkillProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange);

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

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kAdd);

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

	OnSkillProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange);

	return NewResultSPtr;
}

FGameplayTag FModifyItemProxyStrategy_Coin::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Coin;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_Coin::FindByType(
	const FGameplayTag& ProxyType,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	) const
{
	TArray<TSharedPtr<FBasicProxy>> Result;
	if (ProxyTypeMap.Contains(ProxyType))
	{
		Result.Append(ProxyTypeMap[ProxyType]);
	}

	return Result;
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

		OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kAdd, Num);

		return {NewResultSPtr};
	}
	else
	{
		auto NewResultSPtr = MakeShared<FItemProxyType>();

		NewResultSPtr->SetInventoryComponentBase(InventoryComponentPtr);
		NewResultSPtr->InitialProxy(InProxyType);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		ProxyTypeMap.Add(InProxyType, {NewResultSPtr});

		OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange, Num);

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

	ProxyTypeMap.Add(NewResultSPtr->GetProxyType(), {NewResultSPtr});

	OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange, NewResultSPtr->GetNum());

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

	OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange, NewResultSPtr->GetNum());

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

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_Consumable::FindByType(
	const FGameplayTag& ProxyType,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	) const
{
	TArray<TSharedPtr<FBasicProxy>> Result;
	if (ProxyTypeMap.Contains(ProxyType))
	{
		Result.Append(ProxyTypeMap[ProxyType]);
	}

	return Result;
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

		OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kAdd, Num);

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

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		ProxyTypeMap.Add(InProxyType, {NewResultSPtr});

		OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange, Num);

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

	ProxyTypeMap.Add(NewResultSPtr->GetProxyType(), {NewResultSPtr});

	OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange, NewResultSPtr->GetNum());

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

	OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kPropertyChange, NewResultSPtr->GetNum());

	return NewResultSPtr;
}

void FModifyItemProxyStrategy_Consumable::RemoveByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	FModifyItemProxyStrategyBase<FConsumableProxy>::RemoveByRemote(InventoryComponentPtr, RemoteProxySPtr);
	
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

FGameplayTag FModifyItemProxyStrategy_MaterialProxy::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Material;
}

void FModifyItemProxyStrategy_MaterialProxy::FindByID(
	const FGuid& ID,
	const TSharedPtr<FBasicProxy>& FindResultSPtr,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	)
{
	ResultSPtr = nullptr;

	if (FindResultSPtr)
	{
		ResultSPtr = DynamicCastSharedPtr<FItemProxyType>(FindResultSPtr);
	}
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_MaterialProxy::FindByType(
	const FGameplayTag& ProxyType,
	const TObjectPtr<const UInventoryComponentBase>& InventoryComponentPtr
	) const
{
	TArray<TSharedPtr<FBasicProxy>> Result;
	
	for (const auto& Iter : ProxyTypeMap)
	{
		if (Iter.Key.MatchesTag(ProxyType))
		{
			Result.Append(Iter.Value);
		}
	}

	return Result;
}

TArray<TSharedPtr<FBasicProxy>> FModifyItemProxyStrategy_MaterialProxy::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGameplayTag& InProxyType,
	int32 Num
	)
{
	if (InProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Material_PassiveSkill_Experience_Book))
	{
		return AddImp<FExperienceMaterialProxy>(InventoryComponentPtr, InProxyType, Num);
	}

	return {};
}

void FModifyItemProxyStrategy_MaterialProxy::Update(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGuid& InProxyID
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxy(InProxyID))
	{
		InventoryComponentPtr->UpdateInContainer(ProxySPtr);
	}
}

void FModifyItemProxyStrategy_MaterialProxy::RemoveItemProxy(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const FGuid& InProxyID
	)
{
	if (auto ProxySPtr = InventoryComponentPtr->FindProxy(InProxyID))
	{
		InventoryComponentPtr->RemoveFromContainer(ProxySPtr);
	}

	for (auto & Iter : ProxyTypeMap)
	{
		for (int32 Index = 0;Index < Iter.Value.Num(); Index++)
		{
			if (InProxyID == Iter.Value[Index]->GetID())
			{
				Iter.Value.RemoveAt(Index);
				return;
			}
		}
	}
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_MaterialProxy::AddByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
	)
{
	if (InRemoteProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material_PassiveSkill_Experience_Book))
	{
		return AddByRemoteImp<FExperienceMaterialProxy>(InventoryComponentPtr, InRemoteProxySPtr);
	}

	return nullptr;
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_MaterialProxy::UpdateByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& InRemoteProxySPtr
	)
{
	if (InRemoteProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material_PassiveSkill_Experience_Book))
	{
		return UpdateByRemoteImp<FExperienceMaterialProxy>(InventoryComponentPtr, LocalProxySPtr, InRemoteProxySPtr);
	}

	return nullptr;
}

void FModifyItemProxyStrategy_MaterialProxy::RemoveByRemote(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	FModifyItemProxyStrategyIterface::RemoveByRemote(InventoryComponentPtr, RemoteProxySPtr);
	
	OnProxyChanged.ExcuteCallback(DynamicCastSharedPtr<FMaterialProxy>(RemoteProxySPtr), EProxyModifyType::kRemove, 0);
}

TSharedPtr<FBasicProxy> FModifyItemProxyStrategy_MaterialProxy::NetSerialize(
	const FGameplayTag ProxyType,
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Material_PassiveSkill_Experience_Book))
	{
		return NetSerializeImp<FExperienceMaterialProxy>(Ar, Map, bOutSuccess);
	}

	return nullptr;
}
