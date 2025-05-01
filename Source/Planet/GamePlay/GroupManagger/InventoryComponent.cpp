#include "InventoryComponent.h"

#include <GameFramework/PlayerState.h>

#include "CharacterAttributesComponent.h"
#include "Net/UnrealNetwork.h"

#include "GameplayTagsLibrary.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "SceneProxyExtendInfo.h"
#include "ItemProxy_Container.h"
#include "PlanetControllerInterface.h"
#include "ItemProxy_Character.h"
#include "RewardsTD.h"

UInventoryComponent::UInventoryComponent(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::UpdateID(
	const FBasicProxy::IDType& NewID,
	const FBasicProxy::IDType& OldID
	)
{
	if (auto Iter = ProxysMap.Find(OldID))
	{
		ProxysMap.Remove(OldID);
		ProxysMap.Add(NewID, *Iter);
	}
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		Proxy_Container.InventoryComponentPtr = this;
	}

	// 初始化一些需要的Proxy
	AddProxy_Coin(UGameplayTagsLibrary::Proxy_Coin_Regular, 0);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

void UInventoryComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Proxy_Container, COND_None);
}

void UInventoryComponent::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
}

void UInventoryComponent::AddProxy_Pending(
	FGameplayTag ProxyType,
	int32 Num,
	FGuid Guid
	)
{
	if (PendingMap.Contains(Guid))
	{
		if (PendingMap[Guid].Contains(ProxyType))
		{
			PendingMap[Guid][ProxyType] += Num;
		}
		else
		{
			PendingMap[Guid].Add(ProxyType, Num);
		}
	}
	else
	{
		auto SceneProxyExtendInfoPtr = USceneProxyExtendInfoMap::GetInstance()->GetTableRowProxy(ProxyType);
		PendingMap.Add(Guid, {{ProxyType, Num}});
	}
}

void UInventoryComponent::SyncPendingProxy(
	FGuid Guid
	)
{
	if (PendingMap.Contains(Guid))
	{
		for (const auto& Iter : PendingMap)
		{
			for (const auto& SecondIter : Iter.Value)
			{
				AddProxy(SecondIter.Key, SecondIter.Value);
			}
		}
		PendingMap.Remove(Guid);
	}
}

FName UInventoryComponent::ComponentName = TEXT("InventoryComponent");

#if UE_EDITOR || UE_CLIENT
TSharedPtr<FBasicProxy> UInventoryComponent::AddProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	TSharedPtr<FBasicProxy> Result;

	const auto ProxyType = ProxySPtr->GetProxyType();
	ProxySPtr->InventoryComponentPtr = this;

	if (Result = FindProxy(ProxySPtr->GetID()))
	{
		return Result;
	}

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		ProxysMap.Add(ProxySPtr->GetID(), ProxySPtr);
		ProxysAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnWeaponProxyChanged(DynamicCastSharedPtr<FWeaponProxy>(Result), EProxyModifyType::kAdd);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
	{
		ProxysMap.Add(ProxySPtr->GetID(), ProxySPtr);
		ProxysAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnSkillProxyChanged(DynamicCastSharedPtr<FSkillProxy>(Result), EProxyModifyType::kAdd);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		auto CoinProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
		auto TempResult = AddProxy_Coin(ProxyType, CoinProxySPtr->GetOffsetNum());
		Result = TempResult;

		OnCoinProxyChanged(TempResult, EProxyModifyType::kAdd, CoinProxySPtr->GetOffsetNum());
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		auto CoinProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
		auto TempResult = AddProxy_Consumable(ProxyType, CoinProxySPtr->GetOffsetNum());
		Result = TempResult;

		OnConsumableProxyChanged(TempResult, EProxyModifyType::kAdd);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		ProxysMap.Add(ProxySPtr->GetID(), ProxySPtr);
		ProxysAry.Add(ProxySPtr);

		Result = ProxySPtr;
	}

	return Result;
}

TSharedPtr<FBasicProxy> UInventoryComponent::UpdateProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	TSharedPtr<FBasicProxy> Result = nullptr;

	if (!(LocalProxySPtr && RemoteProxySPtr))
	{
		return Result;
	}

	if (LocalProxySPtr->GetProxyType() != RemoteProxySPtr->GetProxyType())
	{
		return Result;
	}

	const auto ProxyType = RemoteProxySPtr->GetProxyType();

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;

		OnWeaponProxyChanged(LeftProxySPtr, EProxyModifyType::kChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;

		OnSkillProxyChanged(LeftProxySPtr, EProxyModifyType::kChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;

		OnSkillProxyChanged(LeftProxySPtr, EProxyModifyType::kChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;

		OnSkillProxyChanged(LeftProxySPtr, EProxyModifyType::kChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCoinProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCoinProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;

		if (LeftProxySPtr->GetOffsetNum() == 0)
		{
			OnCoinProxyChanged(LeftProxySPtr, EProxyModifyType::kChange, LeftProxySPtr->GetOffsetNum());
		}
		else
		{
			OnCoinProxyChanged(LeftProxySPtr, EProxyModifyType::kNumChange, LeftProxySPtr->GetOffsetNum());
		}
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character_Player))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}

	return Result;
}

void UInventoryComponent::SetAllocationCharacterProxy(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID,
	const FGameplayTag& InSocketTag
	)
{
	SetAllocationCharacterProxy_Server(Proxy_ID, CharacterProxy_ID, InSocketTag);
}
#endif

#if UE_EDITOR || UE_SERVER
TSharedPtr<FWeaponProxy> UInventoryComponent::AddProxy_Weapon(
	const FGameplayTag& ProxyType
	)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FWeaponProxy>();

	ResultPtr->InitialProxy(ProxyType);

#if WITH_EDITOR
#endif

	ResultPtr->ProxyType = ProxyType;
	ResultPtr->InventoryComponentPtr = this;
	ResultPtr->WeaponSkillID =
		AddProxy_Skill(ResultPtr->GetTableRowProxy_WeaponExtendInfo()->WeaponSkillProxyType)->GetID();

	ProxysAry.Add(ResultPtr);
	ProxysMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnWeaponProxyChanged(ResultPtr, EProxyModifyType::kAdd);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UInventoryComponent::Update_Weapon(
	const TSharedPtr<FWeaponProxy>& Proxy
	)
{
	auto ResultPtr = Proxy;

#if WITH_EDITOR
#endif


	ProxysAry.Add(ResultPtr);
	ProxysMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponProxyChanged(ResultPtr, EProxyModifyType::kChange);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UInventoryComponent::FindProxy_Weapon(
	const IDType& ID
	) const
{
	auto Iter = ProxysMap.Find(ID);
	if (Iter)
	{
		return DynamicCastSharedPtr<FWeaponProxy>(*Iter);
	}

	return nullptr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::AddProxy_Skill(
	const FGameplayTag& ProxyType
	)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	TSharedPtr<FSkillProxy> ResultPtr = nullptr;
	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		ResultPtr = MakeShared<FWeaponSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
	{
		ResultPtr = MakeShared<FTalentSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
	{
		ResultPtr = MakeShared<FActiveSkillProxy>();
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
	{
		ResultPtr = MakeShared<FPassiveSkillProxy>();
	}
	else
	{
		return nullptr;
	}

#if WITH_EDITOR
#endif

	ResultPtr->InitialProxy(ProxyType);

	ResultPtr->InventoryComponentPtr = this;

	ProxysAry.Add(ResultPtr);
	ProxysMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnSkillProxyChanged(ResultPtr, EProxyModifyType::kAdd);

	return ResultPtr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::FindProxy_Skill(
	const FGameplayTag& ProxyType
	) const
{
	return nullptr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::FindProxy_Skill(
	const IDType& ID
	) const
{
	if (ProxysMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FSkillProxy>(ProxysMap[ID]);
	}
	return nullptr;
}

TSharedPtr<FConsumableProxy> UInventoryComponent::AddProxy_Consumable(
	const FGameplayTag& ProxyType,
	int32 Num
	)
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		auto Ref = DynamicCastSharedPtr<FConsumableProxy>(ProxyTypeMap[ProxyType]);

		Ref->AddNum(Num);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.UpdateItem(Ref);
		}
#endif

		OnConsumableProxyChanged.ExcuteCallback(Ref, EProxyModifyType::kNumChange);

		return Ref;
	}
	else
	{
		check(Num > 0);

		auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

		auto ResultPtr = MakeShared<FConsumableProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->InventoryComponentPtr = this;

		ResultPtr->AddNum(Num);

		ProxysAry.Add(ResultPtr);
		ProxysMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);

		OnConsumableProxyChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kRemove);

		return ResultPtr;
	}
}

TSharedPtr<FConsumableProxy> UInventoryComponent::FindProxy_Consumable(
	const IDType& ID
	) const
{
	if (ProxysMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FConsumableProxy>(ProxysMap[ID]);
	}
	return nullptr;
}

TSharedPtr<FCoinProxy> UInventoryComponent::AddProxy_Coin(
	const FGameplayTag& ProxyType,
	int32 Num /*= 1*/
	)
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		auto Ref = DynamicCastSharedPtr<FCoinProxy>(ProxyTypeMap[ProxyType]);

		Ref->AddNum(Num);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.UpdateItem(Ref);
		}
#endif

		OnCoinProxyChanged.ExcuteCallback(Ref, EProxyModifyType::kAdd, Num);

		return Ref;
	}
	else
	{
		auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

		auto ResultPtr = MakeShared<FCoinProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->InventoryComponentPtr = this;
		ResultPtr->AddNum(Num);

		ProxysAry.Add(ResultPtr);
		ProxysMap.Add(ResultPtr->ID, ResultPtr);
		ProxyTypeMap.Add(ProxyType, ResultPtr);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.AddItem(ResultPtr);
		}
#endif

		OnCoinProxyChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kNumChange, Num);

		return ResultPtr;
	}
}

TSharedPtr<FCoinProxy> UInventoryComponent::FindProxy_Coin(
	const FGameplayTag& ProxyType
	) const
{
	auto Iter = ProxyTypeMap.Find(ProxyType);
	if (Iter)
	{
		return DynamicCastSharedPtr<FCoinProxy>(*Iter);
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponent::GetProxys() const
{
	return ProxysAry;
}
#endif

void UInventoryComponent::AddProxys_Server_Implementation(
	const FGuid& RewardsItemID
	)
{
	const auto RewardsItem = UPAD_RewardsItems::GetInstance()->GetRewardsItem(RewardsItemID);;
	if (RewardsItem.IsValid())
	{
		for (const auto& Proxy : RewardsItem.RewardsMap)
		{
			AddProxy(Proxy.Key, Proxy.Value);
		}
	}
}

void UInventoryComponent::RemoveProxy_Consumable(
	const TSharedPtr<FConsumableProxy>& ProxyPtr,
	int32 Num /*= 1*/
	)
{
	if (ProxyPtr)
	{
		ProxyPtr->AddNum(-Num);

		OnConsumableProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kRemove);
	}
}

const TArray<TSharedPtr<FBasicProxy>>& UInventoryComponent::GetSceneUintAry() const
{
	return ProxysAry;
}

TArray<TSharedPtr<FCharacterProxy>> UInventoryComponent::GetCharacterProxyAry() const
{
	TArray<TSharedPtr<FCharacterProxy>> Result;

	for (auto Iter : ProxysAry)
	{
		if (Iter->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Character))
		{
			auto GroupmateProxyPtr = DynamicCastSharedPtr<FCharacterProxy>(Iter);
			check(GroupmateProxyPtr);
			if (GroupmateProxyPtr)
			{
				Result.Add(GroupmateProxyPtr);
			}
		}
	}

	return Result;
}

TSharedPtr<FCharacterProxy> UInventoryComponent::FindProxy_Character(
	const IDType& ID
	) const
{
	if (ProxysMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FCharacterProxy>(ProxysMap[ID]);
	}

	return nullptr;
}

TSharedPtr<FBasicProxy> UInventoryComponent::AddProxy(
	const FGameplayTag& ProxyType,
	int32 Num
	)
{
	TSharedPtr<FBasicProxy> ResultSPtr = nullptr;

	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		ResultSPtr = AddProxy_Weapon(ProxyType);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
	{
		ResultSPtr = AddProxy_Skill(ProxyType);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		ResultSPtr = AddProxy_Coin(ProxyType, Num);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		ResultSPtr = AddProxy_Consumable(ProxyType, Num);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		ResultSPtr = AddProxy_Character(ProxyType);
	}

	return ResultSPtr;
}

TSharedPtr<FBasicProxy> UInventoryComponent::FindProxy(
	const IDType& ID
	) const
{
	if (ProxysMap.Contains(ID))
	{
		return ProxysMap[ID];
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponent::GetProxys(
	const FGameplayTag& ProxyType
	) const
{
	TArray<TSharedPtr<FBasicProxy>> Result;

	for (const auto& Iter : ProxysAry)
	{
		if (Iter && Iter->GetProxyType().MatchesTag(ProxyType))
		{
			Result.Add(Iter);
		}
	}

	return Result;
}

TSharedPtr<IProxy_Allocationble> UInventoryComponent::FindAllocationableProxy(
	const IDType& ID
	) const
{
	return DynamicCastSharedPtr<IProxy_Allocationble>(FindProxy(ID));
}

TSharedPtr<IProxy_Allocationble> UInventoryComponent::FindProxy_BySocket(
	const FCharacterSocket& Socket
	) const
{
	return FindAllocationableProxy(Socket.GetAllocationedProxyID());
}

void UInventoryComponent::SetAllocationCharacterProxy_Server_Implementation(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID,
	const FGameplayTag& InSocketTag
	)
{
	auto ProxySPtr = FindAllocationableProxy(Proxy_ID);
	if (!ProxySPtr)
	{
		return;
	}

	auto CharacterProxySPtr = FindProxy_Character(CharacterProxy_ID);
	if (CharacterProxySPtr)
	{
		ProxySPtr->SetAllocationCharacterProxy(CharacterProxySPtr, InSocketTag);
	}
	else
	{
		ProxySPtr->ResetAllocationCharacterProxy();
	}
}

TSharedPtr<FCharacterProxy> UInventoryComponent::InitialOwnerCharacterProxy(
	ACharacterBase* OwnerCharacterPtr
	)
{
	Proxy_Container.InventoryComponentPtr = this;

	if (!OwnerCharacterPtr)
	{
		return nullptr;
	}

	// 是否已存在？
	const auto ID = OwnerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterID();
	if (ProxysMap.Contains(ID))
	{
		// 使用指定的ID
		auto CharacterProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxysMap[ID]);

		// CharacterProxySPtr->InitialProxy(OwnerCharacterPtr->GetCharacterAttributesComponent()->CharacterCategory);
		CharacterProxySPtr->ProxyCharacterPtr = OwnerCharacterPtr;
		CharacterProxySPtr->InventoryComponentPtr = this;

#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
		}
#endif

		return CharacterProxySPtr;
	}
	else
	{
		// 使用指定的ID
		auto CharacterProxySPtr = MakeShared<FCharacterProxy>(
		                                                      OwnerCharacterPtr->GetCharacterAttributesComponent()->
		                                                      GetCharacterID()
		                                                     );

		CharacterProxySPtr->InitialProxy(OwnerCharacterPtr->GetCharacterAttributesComponent()->CharacterCategory);
		CharacterProxySPtr->ProxyCharacterPtr = OwnerCharacterPtr;
		CharacterProxySPtr->InventoryComponentPtr = this;

		ProxysAry.Add(CharacterProxySPtr);
		ProxysMap.Add(CharacterProxySPtr->ID, CharacterProxySPtr);
		Proxy_Container.AddItem(CharacterProxySPtr);

#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
		}
#endif

		return CharacterProxySPtr;
	}
}

TSharedPtr<FCharacterProxy> UInventoryComponent::AddProxy_Character(
	const FGameplayTag& ProxyType
	)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	TSharedPtr<FCharacterProxy> ResultPtr = nullptr;

	if (UGameplayTagsLibrary::Proxy_Character_Player == ProxyType)
	{
		ResultPtr = MakeShared<FCharacterProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->InventoryComponentPtr = this;

		ProxysAry.Add(ResultPtr);
		ProxysMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);
	}
	else
	{
		ResultPtr = MakeShared<FCharacterProxy>();

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->InventoryComponentPtr = this;

		ProxysAry.Add(ResultPtr);
		ProxysMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);
	}

	return ResultPtr;
}
