#include "HoldingItemsComponent.h"

#include <GameFramework/PlayerState.h>
#include "Net/UnrealNetwork.h"

#include "GameplayTagsLibrary.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "SceneProxyExtendInfo.h"
#include "ItemProxy_Container.h"
#include "PlanetControllerInterface.h"
#include "ItemProxy_Character.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
}

void UHoldingItemsComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		Proxy_Container.HoldingItemsComponentPtr = this;
	}
}

void UHoldingItemsComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
}

void UHoldingItemsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Proxy_Container, COND_None);
	DOREPLIFETIME_CONDITION(ThisClass, CharacterProxyID_Container, COND_InitialOnly);
}

void UHoldingItemsComponent::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
}

void UHoldingItemsComponent::OnRep_GetCharacterProxyID()
{
	CharacterProxySPtr = FindProxy_Character(CharacterProxyID_Container);
}

void UHoldingItemsComponent::UpdateSocket_Server_Implementation(
	const FGuid& CharacterProxyID,
	const FCharacterSocket& Socket
)
{
	UpdateSocket(FindProxy_Character(CharacterProxyID), Socket);
}

void UHoldingItemsComponent::AddProxy_Pending(FGameplayTag ProxyType, int32 Num, FGuid Guid)
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
		PendingMap.Add(Guid, {{ProxyType, 1}});
	}
}

void UHoldingItemsComponent::SyncPendingProxy(FGuid Guid)
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

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");

#if UE_EDITOR || UE_CLIENT
TSharedPtr<FBasicProxy> UHoldingItemsComponent::AddProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
	TSharedPtr<FBasicProxy> Result;

	const auto ProxyType = ProxySPtr->GetProxyType();
	ProxySPtr->HoldingItemsComponentPtr = this;

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

TSharedPtr<FBasicProxy> UHoldingItemsComponent::UpdateProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr)
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
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(CharacterProxySPtr->GetID()));
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

void UHoldingItemsComponent::SetAllocationCharacterProxy(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID,
	const FGameplayTag& InSocketTag
)
{
	SetAllocationCharacterProxy_Server(Proxy_ID, CharacterProxy_ID, InSocketTag);
}
#endif

#if UE_EDITOR || UE_SERVER
TSharedPtr<FWeaponProxy> UHoldingItemsComponent::AddProxy_Weapon(const FGameplayTag& ProxyType)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FWeaponProxy>();

	ResultPtr->InitialProxy(ProxyType);

#if WITH_EDITOR
#endif

	ResultPtr->ProxyType = ProxyType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;
	ResultPtr->WeaponSkillID =
		AddProxy_Skill(ResultPtr->GetTableRowProxy_WeaponExtendInfo()->WeaponSkillProxyType)->GetID();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnWeaponProxyChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UHoldingItemsComponent::Update_Weapon(const TSharedPtr<FWeaponProxy>& Proxy)
{
	auto ResultPtr = Proxy;

#if WITH_EDITOR
#endif

	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponProxyChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UHoldingItemsComponent::FindProxy_Weapon(const IDType& ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return DynamicCastSharedPtr<FWeaponProxy>(*Iter);
	}

	return nullptr;
}

TSharedPtr<FSkillProxy> UHoldingItemsComponent::AddProxy_Skill(const FGameplayTag& ProxyType)
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

	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnSkillProxyChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FSkillProxy> UHoldingItemsComponent::Update_Skill(const TSharedPtr<FSkillProxy>& ProxySPtr)
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

TSharedPtr<FSkillProxy> UHoldingItemsComponent::FindProxy_Skill(const FGameplayTag& ProxyType)
{
	return nullptr;
}

TSharedPtr<FSkillProxy> UHoldingItemsComponent::FindProxy_Skill(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FSkillProxy>(SceneMetaMap[ID]);
	}
	return nullptr;
}

TSharedPtr<FConsumableProxy> UHoldingItemsComponent::AddProxy_Consumable(const FGameplayTag& ProxyType, int32 Num)
{
	check(Num > 0);

	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FConsumableProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->InitialProxy(ProxyType);

	ResultPtr->Num = Num;
	ResultPtr->ProxyType = ProxyType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnConsumableProxyChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kRemove);

	return ResultPtr;
}

TSharedPtr<FToolProxy> UHoldingItemsComponent::AddProxy_ToolProxy(const FGameplayTag& ProxyType)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	auto ResultPtr = MakeShared<FToolProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->InitialProxy(ProxyType);

	ResultPtr->ProxyType = ProxyType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	return ResultPtr;
}

TSharedPtr<FCoinProxy> UHoldingItemsComponent::AddProxy_Coin(const FGameplayTag& ProxyType, int32 Num /*= 1*/)
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

		ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
		ResultPtr->HoldingItemsComponentPtr = this;
		ResultPtr->Num = Num;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		CoinProxyMap.Add(ProxyType, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);

		OnCoinProxyChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

TSharedPtr<FCoinProxy> UHoldingItemsComponent::FindProxy_Coin(const FGameplayTag& ProxyType) const
{
	auto Iter = CoinProxyMap.Find(ProxyType);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

TArray<TSharedPtr<FBasicProxy>> UHoldingItemsComponent::GetProxys() const
{
	return SceneToolsAry;
}

void UHoldingItemsComponent::RemoveProxy_Consumable(const TSharedPtr<FConsumableProxy>& ProxyPtr, int32 Num /*= 1*/)
{
	if (ProxyPtr)
	{
		ProxyPtr->Num -= Num;

		OnConsumableProxyChanged.ExcuteCallback(ProxyPtr, EProxyModifyType::kRemove);
	}
}

const TArray<TSharedPtr<FBasicProxy>>& UHoldingItemsComponent::GetSceneUintAry() const
{
	return SceneToolsAry;
}

const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& UHoldingItemsComponent::GetCoinUintAry() const
{
	return CoinProxyMap;
}

TArray<TSharedPtr<FCharacterProxy>> UHoldingItemsComponent::GetCharacterProxyAry() const
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

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::GetOwnerCharacterProxy() const
{
	return CharacterProxySPtr;
}

void UHoldingItemsComponent::UpdateSocket(
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

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::FindProxy_Character(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FCharacterProxy>(SceneMetaMap[ID]);
	}

	if (CharacterProxySPtr && (CharacterProxySPtr->GetID() == ID))
	{
		return CharacterProxySPtr;
	}

	return nullptr;
}

TSharedPtr<FBasicProxy> UHoldingItemsComponent::AddProxy(const FGameplayTag& ProxyType, int32 Num)
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
		ResultSPtr = AddProxy_Consumable(ProxyType);
	}
	else if (ProxyType.MatchesTag(UGameplayTagsLibrary::Proxy_Character))
	{
		ResultSPtr = AddProxy_Character(ProxyType);
	}

	return ResultSPtr;
}

TSharedPtr<FBasicProxy> UHoldingItemsComponent::FindProxy(const IDType& ID)
{
	if (SceneMetaMap.Contains(ID))
	{
		return SceneMetaMap[ID];
	}

	return nullptr;
}

TSharedPtr<FAllocationbleProxy> UHoldingItemsComponent::FindAllocationableProxy(const IDType& ID)
{
	return DynamicCastSharedPtr<FAllocationbleProxy>(FindProxy(ID));
}

TSharedPtr<FAllocationbleProxy> UHoldingItemsComponent::FindProxy_BySocket(const FCharacterSocket& Socket)
{
	return FindAllocationableProxy(Socket.AllocationedProxyID);
}

void UHoldingItemsComponent::SetAllocationCharacterProxy_Server_Implementation(
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
		auto CharacterSocket= AllocationCharacterSPtr->FindSocket(PrevSocketTag);
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

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::InitialOwnerCharacterProxy(ACharacterBase* OwnerCharacterPtr)
{
	Proxy_Container.HoldingItemsComponentPtr = this;

#if UE_EDITOR
	if (GetNetMode() == NM_Client)
	{
		CharacterProxySPtr = MakeShared<FCharacterProxy>(CharacterProxyID_Container);
	}
	else
	{
		CharacterProxySPtr = MakeShared<FCharacterProxy>();
	}
#elif UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		CharacterProxySPtr = MakeShared<FCharacterProxy>(CharacterProxyID_Container);
	}
#else
	CharacterProxySPtr = MakeShared<FCharacterProxy>();
#endif

	CharacterProxySPtr->InitialProxy(UGameplayTagsLibrary::Proxy_Character_Player);
	CharacterProxySPtr->ProxyCharacterPtr = OwnerCharacterPtr;
	CharacterProxySPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	CharacterProxySPtr->HoldingItemsComponentPtr = this;

	SceneToolsAry.Add(CharacterProxySPtr);
	SceneMetaMap.Add(CharacterProxySPtr->ID, CharacterProxySPtr);

	Proxy_Container.AddItem(CharacterProxySPtr);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		CharacterProxyID_Container = CharacterProxySPtr->ID;
	}
#endif

	return CharacterProxySPtr;
}

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::AddProxy_Character(const FGameplayTag& ProxyType)
{
	auto SceneProxyExtendInfoPtr = GetTableRowProxy(ProxyType);

	TSharedPtr<FCharacterProxy> ResultPtr = nullptr;

	if (UGameplayTagsLibrary::Proxy_Character_Player == ProxyType)
	{
		ResultPtr = MakeShared<FCharacterProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
		ResultPtr->HoldingItemsComponentPtr = this;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);
	}
	else
	{
		ResultPtr = MakeShared<FCharacterProxy>();

		ResultPtr->InitialProxy(ProxyType);

		ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
		ResultPtr->HoldingItemsComponentPtr = this;

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);
	}

	return ResultPtr;
}

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::Update_Character(const TSharedPtr<FCharacterProxy>& ProxySPtr)
{
	SceneMetaMap.Add(ProxySPtr->ID, ProxySPtr);
	SceneToolsAry.Add(ProxySPtr);

	OnGroupmateProxyChanged(ProxySPtr, true);

	return ProxySPtr;
}
#endif