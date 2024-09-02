
#include "HoldingItemsComponent.h"

#include <GameFramework/PlayerState.h>

#include "GameplayTagsSubSystem.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"
#include "SceneUnitExtendInfo.h"
#include "SceneUnitContainer.h"
#include "PlanetControllerInterface.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

TSharedPtr<FSceneUnitContainer> UHoldingItemsComponent::GetSceneUnitContainer()
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr && OwnerPtr->GetCharacterUnit())
	{
		return OwnerPtr->GetCharacterUnit()->SceneUnitContainer;
	}

	// 创建临时的 
	return MakeShared<FSceneUnitContainer>();
}

void UHoldingItemsComponent::BeginPlay()
{
	Super::BeginPlay();
}

TSharedPtr<FBasicProxy>  UHoldingItemsComponent::AddUnit(FGameplayTag UnitType, int32 Num)
{
	TSharedPtr<FBasicProxy> Result;
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		Result = GetSceneUnitContainer()->AddUnit(UnitType, Num);

		if (GetOwner<FOwnerType>()->GetCharacterUnit())
		{
			Result->OwnerCharacterUnitPtr = GetOwner<FOwnerType>()->GetCharacterUnit().ToWeakPtr();
		}
	}

	return Result;
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
