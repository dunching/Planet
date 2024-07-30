
#include "HoldingItemsComponent.h"

#include <GameFramework/PlayerState.h>

#include "GameplayTagsSubSystem.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "SceneUnitExtendInfo.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

FSceneUnitContainer& UHoldingItemsComponent::GetHoldItemProperty()
{
	return HoldItemProperty;
}

void UHoldingItemsComponent::BeginPlay()
{
	Super::BeginPlay();
}

UBasicUnit* UHoldingItemsComponent::AddUnit(FGameplayTag UnitType, int32 Num)
{
	auto CharacterPtr = GetOwner<ACharacterBase>();
	if (CharacterPtr)
	{
		if (
			UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Skill) ||
			UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Weapon) ||
			UnitType.MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Coin)
			)
		{
			return CharacterPtr->GetPlayerState<APlanetPlayerState>()->GetSceneUnitContainer().AddUnit(UnitType, Num);
		}
		else
		{
			return GetHoldItemProperty().AddUnit(UnitType, Num);
		}
	}
	return nullptr;
}

void UHoldingItemsComponent::AddUnit_Apending(FGameplayTag UnitType, FGuid Guid)
{
	if (SkillUnitApendingMap.Contains(Guid))
	{
		if (SkillUnitApendingMap[Guid].Contains(UnitType))
		{
			SkillUnitApendingMap[Guid][UnitType]++;
		}
		else
		{
			SkillUnitApendingMap[Guid].Add(UnitType, 1);
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

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");
