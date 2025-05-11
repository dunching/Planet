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
				AddProxyNum(SecondIter.Key, SecondIter.Value);
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
		auto RemoteProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
		auto TempResult = AddProxy_Weapon(ProxyType);
		TempResult->UpdateByRemote(RemoteProxySPtr);
		Result = TempResult;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
	{
		auto RemoteProxySPtr = DynamicCastSharedPtr<FSkillProxy>(ProxySPtr);
		auto TempResult = AddProxy_Skill(ProxyType);
		TempResult->UpdateByRemote(RemoteProxySPtr);
		Result = TempResult;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		auto RemoteProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
		auto TempResult = AddProxy_Coin(ProxyType, RemoteProxySPtr->GetOffsetNum());
		TempResult->UpdateByRemote(RemoteProxySPtr);
		Result = TempResult;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		auto CoinProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
		auto TempResult = AddProxy_Consumable(ProxyType, CoinProxySPtr->GetOffsetNum());
		TempResult->UpdateByRemote(CoinProxySPtr);
		Result = TempResult;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		auto CoinProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
		auto TempResult = AddProxy_Character(ProxyType);
		TempResult->UpdateByRemote(CoinProxySPtr);
		Result = TempResult;
	}

	return Result;
}

void UInventoryComponent::RemoveProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& ProxySPtr
	)
{
	const auto ProxyType = ProxySPtr->GetProxyType();

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		auto TargetProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
		RemoveProxy_Weapon(TargetProxySPtr);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
	{
		auto TargetProxySPtr = DynamicCastSharedPtr<FSkillProxy>(ProxySPtr);
		RemoveProxy_Skill(TargetProxySPtr);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		auto TargetProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
		RemoveProxy_Coin(TargetProxySPtr, TargetProxySPtr->GetNum());
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		auto TargetProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
		RemoveProxy_Consumable(TargetProxySPtr, TargetProxySPtr->GetNum());
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
	}
}

void UInventoryComponent::UpdateProxy_SyncHelper(
	const TSharedPtr<FBasicProxy>& LocalProxySPtr,
	const TSharedPtr<FBasicProxy>& RemoteProxySPtr
	)
{
	if (!(LocalProxySPtr && RemoteProxySPtr))
	{
		return;
	}

	if (LocalProxySPtr->GetProxyType() != RemoteProxySPtr->GetProxyType())
	{
		return;
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

		OnWeaponProxyChanged(LeftProxySPtr, EProxyModifyType::kPropertyChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		OnSkillProxyChanged(LeftProxySPtr, EProxyModifyType::kPropertyChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		OnSkillProxyChanged(LeftProxySPtr, EProxyModifyType::kPropertyChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		OnSkillProxyChanged(LeftProxySPtr, EProxyModifyType::kPropertyChange);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCoinProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCoinProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		if (LeftProxySPtr->GetOffsetNum() == 0)
		{
			OnCoinProxyChanged(LeftProxySPtr, EProxyModifyType::kPropertyChange, LeftProxySPtr->GetOffsetNum());
		}
		else
		{
			OnCoinProxyChanged(LeftProxySPtr, EProxyModifyType::kNumChanged, LeftProxySPtr->GetOffsetNum());
		}
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		if (LeftProxySPtr->GetOffsetNum() == 0)
		{
			OnConsumableProxyChanged(LeftProxySPtr, EProxyModifyType::kPropertyChange, LeftProxySPtr->GetOffsetNum());
		}
		else
		{
			OnConsumableProxyChanged(LeftProxySPtr, EProxyModifyType::kNumChanged, LeftProxySPtr->GetOffsetNum());
		}
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character_Player))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(RemoteProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(LocalProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);
	}
}

void UInventoryComponent::SetAllocationCharacterProxy(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID,
	const FGameplayTag& InSocketTag
	)
{
	SetAllocationCharacterProxy_Server(Proxy_ID, CharacterProxy_ID, InSocketTag);
}

void UInventoryComponent::UpdateSocket(
		const FGuid& CharacterProxy_ID,
	const FCharacterSocket& Socket
	)
{
	UpdateSocket_Server(CharacterProxy_ID, Socket);
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
	
#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		ResultPtr->WeaponSkillID =
			AddProxy_Skill(ResultPtr->GetTableRowProxy_WeaponExtendInfo()->WeaponSkillProxyType)->GetID();
	}
#endif

	ProxysAry.Add(ResultPtr);
	ProxysMap.Add(ResultPtr->ID, ResultPtr);

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.AddItem(ResultPtr);
	}
#endif

	OnWeaponProxyChanged(ResultPtr, EProxyModifyType::kNumChanged);

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

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.UpdateItem(ResultPtr);
	}
#endif

	OnWeaponProxyChanged(ResultPtr, EProxyModifyType::kPropertyChange);

	return ResultPtr;
}

void UInventoryComponent::RemoveProxy_Weapon(
	const TSharedPtr<FWeaponProxy>& ProxyPtr
	)
{
	for (int32 Index = 0; Index < ProxysAry.Num(); Index++)
	{
		if (ProxysAry[Index] == ProxyPtr)
		{
			ProxysAry.RemoveAt(Index);
			break;
		}
	}
	ProxysMap.Remove(ProxyPtr->GetID());

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.RemoveItem(ProxyPtr);
	}
#endif

	OnWeaponProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kRemove);
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

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.AddItem(ResultPtr);
	}
#endif

	OnSkillProxyChanged(ResultPtr, EProxyModifyType::kNumChanged);

	return ResultPtr;
}

void UInventoryComponent::RemoveProxy_Skill(
	const TSharedPtr<FSkillProxy>& ProxyPtr
	)
{
	for (int32 Index = 0; Index < ProxysAry.Num(); Index++)
	{
		if (ProxysAry[Index] == ProxyPtr)
		{
			ProxysAry.RemoveAt(Index);
			break;
		}
	}
	ProxysMap.Remove(ProxyPtr->GetID());

#if UE_EDITOR || UE_SERVER
	if (GetOwnerRole() == ROLE_Authority)
	{
		Proxy_Container.RemoveItem(ProxyPtr);
	}
#endif

	OnSkillProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kRemove);
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
	int32 Num,
	const IDType& ID
	)
{
	if (ProxyTypeMap.Contains(ProxyType))
	{
		auto Ref = DynamicCastSharedPtr<FConsumableProxy>(ProxyTypeMap[ProxyType]);

		Ref->ModifyNum(Num);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.UpdateItem(Ref);
		}
#endif

		OnConsumableProxyChanged.ExcuteCallback(Ref, EProxyModifyType::kNumChanged, Num);

		return Ref;
	}
	else
	{
		check(Num > 0);

		auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

		auto ResultPtr = MakeShared<FConsumableProxy>();
		if (ID.IsValid())
		{
			ResultPtr->ID = ID;
		}

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->InventoryComponentPtr = this;

		ResultPtr->ModifyNum(Num);

		ProxysAry.Add(ResultPtr);
		ProxysMap.Add(ResultPtr->ID, ResultPtr);
		ProxyTypeMap.Add(ProxyType, ResultPtr);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.AddItem(ResultPtr);
		}
#endif

		OnConsumableProxyChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kNumChanged, Num);

		return ResultPtr;
	}
}

void UInventoryComponent::RemoveProxy_Consumable(
	const TSharedPtr<FConsumableProxy>& ProxyPtr,
	int32 Num /*= 1*/
	)
{
	if (ProxyPtr)
	{
		const auto CurrentNum = ProxyPtr->GetNum();
		if (Num < CurrentNum)
		{
			ProxyPtr->ModifyNum(-Num);

#if UE_EDITOR || UE_SERVER
			if (GetOwnerRole() == ROLE_Authority)
			{
				Proxy_Container.UpdateItem(ProxyPtr);
			}
#endif

			OnConsumableProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kNumChanged, Num);
		}
		else
		{
			ProxyPtr->ModifyNum(-Num);

			for (int32 Index = 0; Index < ProxysAry.Num(); Index++)
			{
				if (ProxysAry[Index] == ProxyPtr)
				{
					ProxysAry.RemoveAt(Index);
					break;
				}
			}
			ProxysMap.Remove(ProxyPtr->GetID());
			ProxyTypeMap.Remove(ProxyPtr->GetProxyType());

#if UE_EDITOR || UE_SERVER
			if (GetOwnerRole() == ROLE_Authority)
			{
				Proxy_Container.RemoveItem(ProxyPtr);
			}
#endif

			OnConsumableProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kRemove, Num);
		}
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

		Ref->ModifyNum(Num);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.UpdateItem(Ref);
		}
#endif

		OnCoinProxyChanged.ExcuteCallback(Ref, EProxyModifyType::kNumChanged, Num);

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
		ResultPtr->ModifyNum(Num);

		ProxysAry.Add(ResultPtr);
		ProxysMap.Add(ResultPtr->ID, ResultPtr);
		ProxyTypeMap.Add(ProxyType, ResultPtr);

#if UE_EDITOR || UE_SERVER
		if (GetOwnerRole() == ROLE_Authority)
		{
			Proxy_Container.AddItem(ResultPtr);
		}
#endif

		OnCoinProxyChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kNumChanged, Num);

		return ResultPtr;
	}
}

void UInventoryComponent::RemoveProxy_Coin(
	const TSharedPtr<FCoinProxy>& ProxyPtr,
	int32 Num
	)
{
	if (ProxyPtr)
	{
		const auto CurrentNum = ProxyPtr->GetNum();
		if (Num < CurrentNum)
		{
			ProxyPtr->ModifyNum(-Num);

#if UE_EDITOR || UE_SERVER
			if (GetOwnerRole() == ROLE_Authority)
			{
				Proxy_Container.UpdateItem(ProxyPtr);
			}
#endif

			OnCoinProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kNumChanged, Num);
		}
		else
		{
			// 不要移除这个类型，因为我们可以让金币成为负数或称为0
			ProxyPtr->ModifyNum(-Num);

#if UE_EDITOR || UE_SERVER
			if (GetOwnerRole() == ROLE_Authority)
			{
				Proxy_Container.UpdateItem(ProxyPtr);
			}
#endif

			OnCoinProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kNumChanged, Num);
		}
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
			AddProxyNum(Proxy.Key, Proxy.Value);
		}
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

TArray<TSharedPtr<FBasicProxy>> UInventoryComponent::AddProxyNum(
	const FGameplayTag& ProxyType,
	int32 Num
	)
{
	if (Num <= 0)
	{
		return {};
	}

	TArray<TSharedPtr<FBasicProxy>> ResultAry;

	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		for (int Index = 0; Index < Num; Index++)
		{
			ResultAry.Add(AddProxy_Weapon(ProxyType));
		}
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
	{
		for (int Index = 0; Index < Num; Index++)
		{
			ResultAry.Add(AddProxy_Skill(ProxyType));
		}
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		ResultAry.Add(AddProxy_Coin(ProxyType, Num));
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		ResultAry.Add(AddProxy_Consumable(ProxyType, Num));
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		for (int Index = 0; Index < Num; Index++)
		{
			ResultAry.Add(AddProxy_Character(ProxyType));
		}
	}

	return ResultAry;
}

void UInventoryComponent::RemoveProxyNum(
	const IDType& ID,
	int32 Num
	)
{
	if (Num <= 0)
	{
		return;
	}

	TSharedPtr<FBasicProxy> ResultSPtr = FindProxy(ID);
	if (ResultSPtr)
	{
		const auto ProxyType = ResultSPtr->GetProxyType();

		if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
		{
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
		{
			RemoveProxy_Weapon(DynamicCastSharedPtr<FWeaponProxy>(ResultSPtr));
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
		{
			RemoveProxy_Skill(DynamicCastSharedPtr<FSkillProxy>(ResultSPtr));
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
		{
			RemoveProxy_Coin(DynamicCastSharedPtr<FCoinProxy>(ResultSPtr), Num);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
		{
			RemoveProxy_Consumable(DynamicCastSharedPtr<FConsumableProxy>(ResultSPtr), Num);
		}
		else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
		{
		}
	}
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

void UInventoryComponent::UpdateSocket_Server_Implementation(
	const FGuid& CharacterProxy_ID,
	const FCharacterSocket& Socket
	)
{
	auto CharacterProxySPtr = FindProxy_Character(CharacterProxy_ID);
	if (!CharacterProxySPtr)
	{
		return;
	}

	CharacterProxySPtr->UpdateSocket(Socket);
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
