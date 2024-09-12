
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

void UHoldingItemsComponent::OnRep_ProxyChanged()
{

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

TSharedPtr<FWeaponProxy> UHoldingItemsComponent::AddUnit_Weapon(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FWeaponProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;
	ResultPtr->FirstSkill = DynamicCastSharedPtr<FWeaponSkillProxy>(
		AddUnit_Skill(ResultPtr->GetTableRowUnit_WeaponExtendInfo()->WeaponSkillUnitType)
	);

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

	ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;

	SceneToolsAry.Add(ResultPtr);
	SceneMetaMap.Add(ResultPtr->ID, ResultPtr);

	OnWeaponUnitChanged(ResultPtr, true);

	return ResultPtr;
}

TSharedPtr<FWeaponProxy> UHoldingItemsComponent::FindUnit_Weapon(const FGameplayTag& UnitType)
{
	return nullptr;
}

TSharedPtr<FSkillProxy>  UHoldingItemsComponent::AddUnit_Skill(const FGameplayTag& UnitType)
{
	if (SkillUnitMap.Contains(UnitType))
	{
		// 

		return SkillUnitMap[UnitType];
	}
	else
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

#if WITH_EDITOR
#endif

		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;

		ResultPtr->InitialUnit();

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		SkillUnitMap.Add(UnitType, ResultPtr);

		Proxy_Container.AddItem(ResultPtr);

		OnSkillUnitChanged(ResultPtr, true);

		return ResultPtr;
	}
}

TSharedPtr<FSkillProxy>  UHoldingItemsComponent::Update_Skill(const TSharedPtr<FSkillProxy>& UnitSPtr)
{
	SceneToolsAry.Add(UnitSPtr);
	SceneMetaMap.Add(UnitSPtr->ID, UnitSPtr);
	SkillUnitMap.Add(UnitSPtr->UnitType, UnitSPtr);

	OnSkillUnitChanged(UnitSPtr, true);

	return UnitSPtr;
}

TSharedPtr<FSkillProxy> UHoldingItemsComponent::FindUnit_Skill(const FGameplayTag& UnitType)
{
	auto Iter = SkillUnitMap.Find(UnitType);
	if (Iter)
	{
		return *Iter;
	}

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

	if (ConsumablesUnitMap.Contains(UnitType))
	{
		auto Ref = ConsumablesUnitMap[UnitType];

		Ref->Num += Num;

		OnConsumableUnitChanged.ExcuteCallback(Ref, true, Num);

		return Ref;
	}
	else
	{
		auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

		auto ResultPtr = MakeShared<FConsumableProxy>();

#if WITH_EDITOR
#endif

		ResultPtr->Num = Num;
		ResultPtr->UnitType = UnitType;
		ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;

		ResultPtr->InitialUnit();

		SceneToolsAry.Add(ResultPtr);
		SceneMetaMap.Add(ResultPtr->ID, ResultPtr);
		ConsumablesUnitMap.Add(UnitType, ResultPtr);

		OnConsumableUnitChanged.ExcuteCallback(ResultPtr, true, Num);

		return ResultPtr;
	}
}

TSharedPtr<FToolProxy> UHoldingItemsComponent::AddUnit_ToolUnit(const FGameplayTag& UnitType)
{
	auto SceneUnitExtendInfoPtr = GetTableRowUnit(UnitType);

	auto ResultPtr = MakeShared<FToolProxy>();

#if WITH_EDITOR
#endif

	ResultPtr->UnitType = UnitType;
	ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;

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
		ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;
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

		OnConsumableUnitChanged.ExcuteCallback(UnitPtr, false, Num);
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
	CharacterProxySPtr->ProxyCharacterPtr = GetOwner<ACharacterBase>();

	CharacterProxySPtr->InitialUnit();

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
		ResultPtr->OwnerCharacterUnitPtr = CharacterProxySPtr;

		ResultPtr->InitialUnit();

		Proxy_Container.AddItem(ResultPtr);
	}

	return ResultPtr;
}

TSharedPtr<FCharacterProxy> UHoldingItemsComponent::Update_Character(const TSharedPtr<FCharacterProxy>& UnitSPtr)
{
	CharacterUnitMap.Add(UnitSPtr->GetID(), UnitSPtr);
	SceneMetaMap.Add(UnitSPtr->ID, UnitSPtr);
	SceneToolsAry.Add(UnitSPtr);

	OnGroupmateUnitChanged(UnitSPtr, true);

	return UnitSPtr;
}
