
#include "HoldingItemsComponent.h"

#include <GameFramework/PlayerState.h>
#include "Net/UnrealNetwork.h"

#include "GameplayTagsSubSystem.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "SceneUnitExtendInfo.h"
#include "SceneUnitContainer.h"
#include "PlanetControllerInterface.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
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

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_SkipReplay;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Proxy_Container, Params);
}

void UHoldingItemsComponent::AddUnit_Apending(FGameplayTag UnitType, int32 Num, FGuid Guid)
{
	if (SkillUnitApendingMap.Contains(Guid))
	{
		if (SkillUnitApendingMap[Guid].Contains(UnitType))
		{
			SkillUnitApendingMap[Guid][UnitType] += Num;
		}
		else
		{
			SkillUnitApendingMap[Guid].Add(UnitType, Num);
		}
	}
	else
	{
		auto SceneUnitExtendInfoPtr = USceneUnitExtendInfoMap::GetInstance()->GetTableRowUnit(UnitType);
		SkillUnitApendingMap.Add(Guid, { {UnitType, 1} });
	}
}

void UHoldingItemsComponent::SyncApendingUnit(FGuid Guid)
{
	if (SkillUnitApendingMap.Contains(Guid))
	{
		for (const auto& Iter : SkillUnitApendingMap)
		{
			for (const auto& SecondIter : Iter.Value)
			{
				AddProxy(SecondIter.Key, SecondIter.Value);
			}
		}
		SkillUnitApendingMap.Remove(Guid);
	}
}

void UHoldingItemsComponent::SetAllocationCharacterUnit_Implementation(
	const FGuid& Proxy_ID,
	const FGuid& CharacterProxy_ID
)
{
	auto ProxySPtr = FindProxy(Proxy_ID);
	if (!ProxySPtr)
	{
		return;
	}

	auto TargetCharacterProxySPtr = FindUnit_Character(CharacterProxy_ID);
	if (TargetCharacterProxySPtr)
	{
		ProxySPtr->SetAllocationCharacterUnit(TargetCharacterProxySPtr);
	}
	else
	{
		ProxySPtr->SetAllocationCharacterUnit(nullptr);
	}

	if (ProxySPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
	{
		// 此项是跟 Weapon 绑定的，所以不必复制
	}
	else
	{
		Proxy_Container.UpdateItem(ProxySPtr);
	}
}

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");

#if UE_EDITOR || UE_CLIENT
TSharedPtr<FBasicProxy> UHoldingItemsComponent::AddProxy_SyncHelper(const TSharedPtr<FBasicProxy>& ProxySPtr)
{
	TSharedPtr<FBasicProxy> Result;

	const auto UnitType = ProxySPtr->GetUnitType();
	ProxySPtr->HoldingItemsComponentPtr = this;

	if (Result = FindProxy(ProxySPtr->GetID()))
	{
		return Result;
	}

	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnWeaponUnitChanged(DynamicCastSharedPtr<FWeaponProxy>(Result), true);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnSkillUnitChanged(DynamicCastSharedPtr<FSkillProxy>(Result), true);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
		SceneMetaMap.Add(ProxySPtr->GetID(), ProxySPtr);
		SceneToolsAry.Add(ProxySPtr);

		Result = ProxySPtr;

		OnConsumableUnitChanged(DynamicCastSharedPtr<FConsumableProxy>(Result), EProxyModifyType::kAdd);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
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

	const auto UnitType = ProxySPtr->GetUnitType();

	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FToolProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FToolProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FTalentSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FWeaponSkillProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCoinProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCoinProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FConsumableProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate_Player))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(CharacterProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
	{
		auto RightProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(ProxySPtr);
		auto LeftProxySPtr = DynamicCastSharedPtr<FCharacterProxy>(FindProxy(ProxySPtr->GetID()));
		LeftProxySPtr->UpdateByRemote(RightProxySPtr);

		Result = LeftProxySPtr;
	}

	return Result;
}
#endif

#if UE_EDITOR || UE_SERVER
TSharedPtr<FWeaponProxy> UHoldingItemsComponent::AddUnit_Weapon(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FWeaponProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;
	ResultPtr->WeaponSkillID = 
		AddUnit_Skill(ResultPtr->GetTableRowUnit_WeaponExtendInfo()->WeaponSkillUnitType)->GetID();

	ResultPtr->InitialUnit();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnWeaponUnitChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UHoldingItemsComponent::Update_Weapon(const TSharedPtr<FWeaponProxy>& Unit)
{
	auto ResultPtr = Unit;

#if WITH_EDITOR
#endif

	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponUnitChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UHoldingItemsComponent::FindUnit_Weapon(const IDType& ID)
{
	auto Iter = SceneMetaMap.Find(ID);
	if (Iter)
	{
		return DynamicCastSharedPtr<FWeaponProxy>(*Iter);
	}

	return nullptr;
}

TSharedPtr<FSkillProxy>  UHoldingItemsComponent::AddUnit_Skill(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	TSharedPtr<FSkillProxy> ResultPtr = nullptr;
	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon))
	{
		ResultPtr = MakeShared<FWeaponSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent))
	{
		ResultPtr = MakeShared<FTalentSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Active))
	{
		ResultPtr = MakeShared<FActiveSkillProxy>();
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Passve))
	{
		ResultPtr = MakeShared<FPassiveSkillProxy>();
	}
	else
	{
		return nullptr;
	}

#if WITH_EDITOR
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;

	ResultPtr->InitialUnit();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnSkillUnitChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FSkillProxy>  UHoldingItemsComponent::Update_Skill(const TSharedPtr<FSkillProxy>& UnitSPtr)
{
	if (SceneMetaMap.Contains(UnitSPtr->GetID()))
	{
		// 
		*SceneMetaMap[UnitSPtr->GetID()] = *UnitSPtr;

		OnSkillUnitChanged(UnitSPtr, true);
	}

	SceneToolsAry.Add(UnitSPtr);
	SceneMetaMap.Add(UnitSPtr->ID, UnitSPtr);

	OnSkillUnitChanged(UnitSPtr, true);

	return UnitSPtr;
}

TSharedPtr<FSkillProxy> UHoldingItemsComponent::FindUnit_Skill(const FGameplayTag& UnitType)
{
	return nullptr;
}

TSharedPtr<FSkillProxy> UHoldingItemsComponent::FindUnit_Skill(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FSkillProxy>(SceneMetaMap[ID]);
	}
	return nullptr;
}

TSharedPtr <FConsumableProxy> UHoldingItemsComponent::AddUnit_Consumable(const FGameplayTag& UnitType, int32 Num)
{
	check(Num > 0);

	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FConsumableProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->Num = Num;
	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;

	ResultPtr->InitialUnit();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	OnConsumableUnitChanged.ExcuteCallback(ResultPtr, EProxyModifyType::kRemove);

	return ResultPtr;
}

TSharedPtr<FToolProxy> UHoldingItemsComponent::AddUnit_ToolUnit(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FToolProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
	ResultPtr->HoldingItemsComponentPtr = this;

	ResultPtr->InitialUnit();

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	Proxy_Container.AddItem(ResultPtr);

	return ResultPtr;
}

TSharedPtr<FCoinProxy> UHoldingItemsComponent::AddUnit_Coin(const FGameplayTag& UnitType, int32 Num /*= 1*/)
{
	if (CoinUnitMap.Contains(UnitType))
	{
		auto Ref = CoinUnitMap[UnitType];

		Ref->Num += Num;

		OnCoinUnitChanged.ExcuteCallback(Ref, true, Num);

		return Ref;
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

		auto ResultPtr = MakeShared<FCoinProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();
		ResultPtr->HoldingItemsComponentPtr = this;
		ResultPtr->Num = Num;

		ResultPtr->InitialUnit();

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		CoinUnitMap.Add(UnitType, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);

		OnCoinUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

TSharedPtr<FCoinProxy> UHoldingItemsComponent::FindUnit_Coin(const FGameplayTag& UnitType)const
{
	auto Iter = CoinUnitMap.Find(UnitType);
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

void UHoldingItemsComponent::RemoveUnit_Consumable(const TSharedPtr<FConsumableProxy>& UnitPtr, int32 Num /*= 1*/)
{
	if (UnitPtr)
	{
		UnitPtr->Num -= Num;

		OnConsumableUnitChanged.ExcuteCallback(UnitPtr, EProxyModifyType::kRemove);
	}
}

const TArray<TSharedPtr<FBasicProxy>>& UHoldingItemsComponent::GetSceneUintAry() const
{
	return SceneToolsAry;
}

const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& UHoldingItemsComponent::GetCoinUintAry() const
{
	return CoinUnitMap;
}

TArray<TSharedPtr<FCharacterProxy>> UHoldingItemsComponent::GetCharacterProxyAry() const
{
	TArray<TSharedPtr<FCharacterProxy>> Result;

	for (auto Iter : SceneToolsAry)
	{
		if (Iter->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate))
		{
			auto GroupmateUnitPtr = DynamicCastSharedPtr<FCharacterProxy>(Iter);
			check(GroupmateUnitPtr);
			if (GroupmateUnitPtr)
			{
				Result.Add(GroupmateUnitPtr);
			}
		}
	}

	return Result;
}

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::FindUnit_Character(const IDType& ID) const
{
	if (SceneMetaMap.Contains(ID))
	{
		return DynamicCastSharedPtr<FCharacterProxy>(SceneMetaMap[ID]);
	}

	if (CharacterProxySPtr->GetID() == ID)
	{
		return CharacterProxySPtr;
	}

	return nullptr;
}

TSharedPtr<FBasicProxy> UHoldingItemsComponent::AddProxy(const FGameplayTag& UnitType, int32 Num)
{
	TSharedPtr<FBasicProxy> ResultSPtr = nullptr;

	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
		ResultSPtr = AddUnit_ToolUnit(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon))
	{
		ResultSPtr = AddUnit_Weapon(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill))
	{
		ResultSPtr = AddUnit_Skill(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin))
	{
		ResultSPtr = AddUnit_Coin(UnitType, Num);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
	{
		ResultSPtr = AddUnit_Consumable(UnitType);
	}
	else if (UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->DataSource_Character))
	{
		ResultSPtr = AddUnit_Consumable(UnitType);
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

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::InitialDefaultCharacter()
{
	Proxy_Container.HoldingItemsComponentPtr = this;

	CharacterProxySPtr = MakeShared<FCharacterProxy>();

	CharacterProxySPtr->UnitType = UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate_Player;
	CharacterProxySPtr->ProxyCharacterPtr = GetOwner<FOwnerType>();
	CharacterProxySPtr->InitialUnit();

	SceneToolsAry.Add(CharacterProxySPtr);
	SceneMetaMap.Add(CharacterProxySPtr->ID, CharacterProxySPtr);

	Proxy_Container.AddItem(CharacterProxySPtr);

	return CharacterProxySPtr;
}

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::AddUnit_Character(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	TSharedPtr<FCharacterProxy>  ResultPtr = nullptr;

	if (UGameplayTagsSubSystem::GetInstance()->Unit_GroupMate_Player == UnitType)
	{
		ResultPtr = MakeShared<FCharacterProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacter_ID = CharacterProxySPtr->GetID();

		ResultPtr->InitialUnit();

		Proxy_Container.AddItem(ResultPtr);
	}

	return ResultPtr;
}

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::Update_Character(const TSharedPtr<FCharacterProxy>& UnitSPtr)
{
	SceneMetaMap.Add(UnitSPtr->ID, UnitSPtr);
	SceneToolsAry.Add(UnitSPtr);

	OnGroupmateUnitChanged(UnitSPtr, true);

	return UnitSPtr;
}
#endif