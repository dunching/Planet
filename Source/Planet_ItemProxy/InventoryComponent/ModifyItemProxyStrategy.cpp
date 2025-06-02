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

void FModifyItemProxyStrategy_Character::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(ProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);
}

FGameplayTag FModifyItemProxyStrategy_Weapon::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Weapon;
}

void FModifyItemProxyStrategy_Weapon::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(ProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);
}

FGameplayTag FModifyItemProxyStrategy_WeaponSkill::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Weapon;
}

void FModifyItemProxyStrategy_WeaponSkill::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(ProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);
}

FGameplayTag FModifyItemProxyStrategy_ActiveSkill::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Skill_Active;
}

void FModifyItemProxyStrategy_ActiveSkill::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(ProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);
}

FGameplayTag FModifyItemProxyStrategy_PassveSkill::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Skill_Passve;
}

void FModifyItemProxyStrategy_PassveSkill::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto NewResultSPtr = MakeShared<FItemProxyType>();

	NewResultSPtr->InitialProxy(ProxyType);

	InventoryComponentPtr->AddToContainer(NewResultSPtr);

	OnSkillProxyChanged(NewResultSPtr, EProxyModifyType::kNumChanged);
}

FGameplayTag FModifyItemProxyStrategy_Coin::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Coin;
}

void FModifyItemProxyStrategy_Coin::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	if (auto ProxySPtr = InventoryComponentPtr->FindProxyType(ProxyType))
	{
		auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(ProxySPtr);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->UpdateInContainer(NewResultSPtr);

		OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);
	}
	else
	{
		check(Num > 0);

		auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

		auto NewResultSPtr = MakeShared<FCoinProxy>();

		NewResultSPtr->InitialProxy(ProxyType);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		OnCoinProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);
	}
}

FGameplayTag FModifyItemProxyStrategy_Consumable::GetCanOperationType() const
{
	return UGameplayTagsLibrary::Proxy_Consumables;
}

void FModifyItemProxyStrategy_Consumable::Add(
	const TObjectPtr<UInventoryComponentBase>& InventoryComponentPtr,
	int32 Num
	)
{
	const auto ProxyType = GetCanOperationType();
	if (auto ProxySPtr = InventoryComponentPtr->FindProxyType(ProxyType))
	{
		auto NewResultSPtr = DynamicCastSharedPtr<FItemProxyType>(ProxySPtr);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->UpdateInContainer(NewResultSPtr);

		OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);
	}
	else
	{
		check(Num > 0);

		auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

		auto NewResultSPtr = MakeShared<FItemProxyType>();

		NewResultSPtr->InitialProxy(ProxyType);

		NewResultSPtr->ModifyNum(Num);

		InventoryComponentPtr->AddToContainer(NewResultSPtr);

		OnConsumableProxyChanged.ExcuteCallback(NewResultSPtr, EProxyModifyType::kNumChanged, Num);
	}
}
