
#include "HoldingItemsComponent.h"

#include <GameFramework/PlayerState.h>

#include "GameplayTagsSubSystem.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "SceneUnitExtendInfo.h"
#include "SceneUnitContainer.h"
#include "PlanetControllerInterface.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	SceneUnitContainer = MakeShared<FSceneUnitContainer>();
}

TSharedPtr<FSceneUnitContainer> UHoldingItemsComponent::GetSceneUnitContainer()
{
	return SceneUnitContainer;
}

void UHoldingItemsComponent::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		{
			auto Handle =
				GetSceneUnitContainer()->OnSkillUnitChanged.AddCallback([this](const TSharedPtr < FSkillProxy>& UnitPtr, bool bIsAdd)
					{
						OnSkillUnitChanged(*UnitPtr, bIsAdd);
					});
			Handle->bIsAutoUnregister = false;
		}
		{
			auto Handle =
				GetSceneUnitContainer()->OnWeaponUnitChanged.AddCallback([this](const TSharedPtr < FWeaponProxy>& UnitPtr, bool bIsAdd)
					{
						OnWeaponUnitChanged(*UnitPtr, bIsAdd);
					});
			Handle->bIsAutoUnregister = false;
		}
		{
			auto Handle =
				GetSceneUnitContainer()->OnGroupmateUnitChanged.AddCallback([this](const TSharedPtr < FCharacterProxy>& UnitPtr, bool bIsAdd)
					{
						OnGroupmateUnitChanged(*UnitPtr, bIsAdd);
					});
			Handle->bIsAutoUnregister = false;
		}
	}
#endif
}

void UHoldingItemsComponent::OnSkillUnitChanged_Implementation(
	const FSkillProxy& Proxy,
	bool bIsAdd
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		if (bIsAdd)
		{
			auto Result = GetSceneUnitContainer()->Update_Skill(Proxy);
		}
	}
}

void UHoldingItemsComponent::OnWeaponUnitChanged_Implementation(
	const FWeaponProxy& Proxy,
	bool bIsAdd
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		if (bIsAdd)
		{
			auto Result = GetSceneUnitContainer()->Update_Weapon(Proxy);
		}
	}
}

void UHoldingItemsComponent::OnGroupmateUnitChanged_Implementation(
	const FCharacterProxy& Proxy,
	bool bIsAdd
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		if (bIsAdd)
		{
			auto Result = GetSceneUnitContainer()->Update_Character(Proxy);
		}
	}
}

TSharedPtr<FBasicProxy>  UHoldingItemsComponent::AddUnit(FGameplayTag UnitType, int32 Num)
{
	TSharedPtr<FBasicProxy> Result;
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		Result = GetSceneUnitContainer()->AddUnit(UnitType, Num);

		if (Result && OwnerPtr->GetCharacterUnit())
		{
		}
	}

	return Result;
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
				AddUnit(SecondIter.Key, SecondIter.Value);
			}
		}
		SkillUnitApendingMap.Remove(Guid);
	}
}

void UHoldingItemsComponent::SetAllocationCharacterUnit_Implementation(
	const FBasicProxy& Proxy, 
	const FCharacterProxy& CharacterProxy
)
{
	auto ProxySPtr = Proxy.GetThisSPtr();
	if (!ProxySPtr)
	{
		return;
	}
	auto CharacterProxySPtr = CharacterProxy.GetThisSPtr();
	if (CharacterProxySPtr)
	{
		ProxySPtr->SetAllocationCharacterUnit(CharacterProxySPtr);
	}
	else
	{
		ProxySPtr->SetAllocationCharacterUnit(nullptr);
	}
}

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");
