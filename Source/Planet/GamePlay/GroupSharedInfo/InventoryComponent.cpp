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

UInventoryComponent::UInventoryComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		Proxy_Container.InventoryComponentPtr = this;
	}
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

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Proxy_Container, COND_None);
}

void UInventoryComponent::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
}

void UInventoryComponent::UpdateSocket_Server_Implementation(
	const FGuid& CharacterProxyID,
	const FCharacterSocket& Socket
)
{
	UpdateSocket(FindProxy_Character(CharacterProxyID), Socket);
}

void UInventoryComponent::AddProxy_Pending(FGameplayTag ProxyType, int32 Num, FGuid Guid)
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

void UInventoryComponent::SyncPendingProxy(FGuid Guid)
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
TSharedPtr<FBasicProxy> UInventoryComponent::AddProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr)
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
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnWeaponProxyChanged(DynamicCastSharedPtr<FWeaponProxy>(Result), true);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnSkillProxyChanged(DynamicCastSharedPtr<FSkillProxy>(Result), true);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnConsumableProxyChanged(DynamicCastSharedPtr<FConsumableProxy>(Result), EProxyModifyType::kAdd);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;
	}

	return Result;
}

TSharedPtr<FBasicProxy> UInventoryComponent::UpdateProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
	TSharedPtr<FBasicProxy> Result;

	const auto ProxyType = ProxySPtr->GetProxyType();

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FToolProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FToolProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Passve))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Talent))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Coin))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCoinProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character_Player))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(ProxySPtr->GetID()));
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
TSharedPtr<FWeaponProxy> UInventoryComponent::AddProxy_Weapon(const FGameplayTag& ProxyType)
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

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnWeaponProxyChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UInventoryComponent::Update_Weapon(const TSharedPtr<FWeaponProxy>& Proxy)
{
	auto ResultPtr = Proxy;

#if WITH_EDITOR
#endif


	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponProxyChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UInventoryComponent::FindProxy_Weapon(const IDType& ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return DynamicCastSharedPtr<FWeaponProxy>(*Iter);
	}

	return nullptr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::AddProxy_Skill(const FGameplayTag& ProxyType)
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

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnSkillProxyChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::Update_Skill(const TSharedPtr<FSkillProxy>& ProxySPtr)
{
	if (SceneMetaMap.Contains(ProxySPtr->GetID()))
	{
		// 
		*SceneMetaMap[ProxySPtr->GetID()] = *ProxySPtr;

		OnSkillProxyChanged(ProxySPtr, true);
	}

	SceneToolsAry.Add(ProxySPtr);
	SceneMetaMap.Add(ProxySPtr->ID, ProxySPtr);

	OnSkillProxyChanged(ProxySPtr, true);

	return ProxySPtr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::FindProxy_Skill(const FGameplayTag& ProxyType)
{
	return nullptr;
}

TSharedPtr<FSkillProxy> UInventoryComponent::FindProxy_Skill(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FSkillProxy>(SceneMetaMap[ID]);
	}
	return nullptr;
}

TSharedPtr<FConsumableProxy> UInventoryComponent::AddProxy_Consumable(const FGameplayTag& ProxyType, int32 Num)
{
	check(Num > 0);

	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FConsumableProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->InitialProxy(ProxyType);

	ResultPtr->Num = Num;
	ResultPtr->ProxyType = ProxyType;
	ResultPtr->InventoryComponentPtr = this;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnConsumableProxyChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kRemove);

	return ResultPtr;
}

TSharedPtr<FConsumableProxy> UInventoryComponent::FindProxy_Consumable(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FConsumableProxy>(SceneMetaMap[ID]);
	}
	return nullptr;
}

TSharedPtr<FToolProxy> UInventoryComponent::AddProxy_ToolProxy(const FGameplayTag& ProxyType)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FToolProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->InitialProxy(ProxyType);

	ResultPtr->ProxyType = ProxyType;
	ResultPtr->InventoryComponentPtr = this;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	return ResultPtr;
}

TSharedPtr<FCoinProxy> UInventoryComponent::AddProxy_Coin(const FGameplayTag& ProxyType, int32 Num /*= 1*/)
{
	if (CoinProxyMap.Contains(ProxyType))
	{
		auto Ref = CoinProxyMap[ProxyType];

		Ref->Num += Num;

		OnCoinProxyChanged.ExcuteCallback(Ref, true, Num);

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
		ResultPtr->Num = Num;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		CoinProxyMap.Add(ProxyType, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);

		OnCoinProxyChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

TSharedPtr<FCoinProxy> UInventoryComponent::FindProxy_Coin(const FGameplayTag& ProxyType) const
{
	auto Iter = CoinProxyMap.Find(ProxyType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> UInventoryComponent::GetProxys() const
{
	return SceneToolsAry;
}

void UInventoryComponent::RemoveProxy_Consumable(const TSharedPtr<FConsumableProxy>& ProxyPtr, int32 Num /*= 1*/)
{
	if (ProxyPtr)
	{
		ProxyPtr->Num -= Num;

		OnConsumableProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kRemove);
	}
}

const TArray<TSharedPtr<FBasicProxy>>& UInventoryComponent::GetSceneUintAry() const
{
	return SceneToolsAry;
}

const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& UInventoryComponent::GetCoinUintAry() const
{
	return CoinProxyMap;
}

TArray<TSharedPtr<FCharacterProxy>> UInventoryComponent::GetCharacterProxyAry() const
{
	TArray<TSharedPtr<FCharacterProxy>> Result;

	for (auto Iter : SceneToolsAry)
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

void UInventoryComponent::UpdateSocket(
	const TSharedPtr<FCharacterProxy>& InCharacterProxySPtr,
	const FCharacterSocket& Socket
)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		InCharacterProxySPtr->UpdateSocket(Socket);
		InCharacterProxySPtr->Update2Client();
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		// 本地直接更新一次
		InCharacterProxySPtr->UpdateSocket(Socket);

		// Server
		UpdateSocket_Server(InCharacterProxySPtr->GetID(), Socket);
	}
#endif
}

TSharedPtr<FCharacterProxy> UInventoryComponent::FindProxy_Character(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FCharacterProxy>(SceneMetaMap[ID]);
	}

	return nullptr;
}

TSharedPtr<FBasicProxy> UInventoryComponent::AddProxy(const FGameplayTag& ProxyType, int32 Num)
{
	TSharedPtr<FBasicProxy> ResultSPtr = nullptr;

	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
		ResultSPtr = AddProxy_ToolProxy(ProxyType);
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

TSharedPtr<FBasicProxy> UInventoryComponent::FindProxy(const IDType& ID)
{
	if (SceneMetaMap.Contains(ID))
	{
		return SceneMetaMap[ID];
	}

	return nullptr;
}

TSharedPtr<FAllocationbleProxy> UInventoryComponent::FindAllocationableProxy(const IDType& ID)
{
	return DynamicCastSharedPtr<FAllocationbleProxy>(FindProxy(ID));
}

TSharedPtr<FAllocationbleProxy> UInventoryComponent::FindProxy_BySocket(const FCharacterSocket& Socket)
{
	return FindAllocationableProxy(Socket.AllocationedProxyID);
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

	// 找到这个物品之前被分配的插槽
	const auto AllocationCharacterID = ProxySPtr->GetAllocationCharacterID();
	auto AllocationCharacterSPtr = FindProxy_Character(AllocationCharacterID);
	if (AllocationCharacterSPtr)
	{
		const auto PrevSocketTag = ProxySPtr->GetCurrentSocketTag();
		auto CharacterSocket = AllocationCharacterSPtr->FindSocket(PrevSocketTag);
		auto PrevProxySPtr = FindProxy(CharacterSocket.AllocationedProxyID);
		if (PrevProxySPtr)
		{
			PrevProxySPtr->UnAllocation();
		}
		CharacterSocket.AllocationedProxyID = FGuid();
		UpdateSocket(AllocationCharacterSPtr, CharacterSocket);
	}

	auto TargetCharacterProxySPtr = FindProxy_Character(CharacterProxy_ID);
	if (TargetCharacterProxySPtr)
	{
		ProxySPtr->SetAllocationCharacterProxy(TargetCharacterProxySPtr, InSocketTag);
		ProxySPtr->Allocation();
	}
	else
	{
		ProxySPtr->SetAllocationCharacterProxy(nullptr, InSocketTag);
	}

	if (ProxySPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
	{
		// 此项是跟 Weapon 绑定的，所以不必复制
	}
	else
	{
		Proxy_Container.UpdateItem(ProxySPtr);
	}
}

TSharedPtr<FCharacterProxy> UInventoryComponent::InitialOwnerCharacterProxy(ACharacterBase* OwnerCharacterPtr)
{
	Proxy_Container.InventoryComponentPtr = this;

	// 使用指定的ID
	auto CharacterProxySPtr = MakeShared<FCharacterProxy>(
		OwnerCharacterPtr->GetCharacterAttributesComponent()->GetCharacterID()
		);

	CharacterProxySPtr->InitialProxy(OwnerCharacterPtr->GetCharacterAttributesComponent()->CharacterCategory);
	CharacterProxySPtr->ProxyCharacterPtr = OwnerCharacterPtr;
	CharacterProxySPtr->InventoryComponentPtr = this;

	SceneToolsAry.Add(CharacterProxySPtr);
	SceneMetaMap.Add(CharacterProxySPtr->ID, CharacterProxySPtr);

	Proxy_Container.AddItem(CharacterProxySPtr);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif

	return CharacterProxySPtr;
}

TSharedPtr<FCharacterProxy> UInventoryComponent::AddProxy_Character(const FGameplayTag& ProxyType)
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

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);
	}
	else
	{
		ResultPtr = MakeShared<FCharacterProxy>();

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->InventoryComponentPtr = this;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);
	}

	return ResultPtr;
}

TSharedPtr<FCharacterProxy> UInventoryComponent::Update_Character(const TSharedPtr<FCharacterProxy>& ProxySPtr)
{
	SceneMetaMap.Add(ProxySPtr->ID, ProxySPtr);
	SceneToolsAry.Add(ProxySPtr);

	OnGroupmateProxyChanged(ProxySPtr, true);

	return ProxySPtr;
}
#endif
