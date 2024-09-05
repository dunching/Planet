
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
	}
#endif
}

void UHoldingItemsComponent::OnSkillUnitChanged_Implementation(
	const FSkillProxy& Skill,
	bool bIsAdd
)
{
	auto OwnerPtr = GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		if (bIsAdd)
		{
			auto Result = GetSceneUnitContainer()->AddUnit_Skill(Skill);
			Result->OwnerCharacterUnitPtr = OwnerPtr->GetCharacterUnit().ToWeakPtr();
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
			Result->OwnerCharacterUnitPtr = OwnerPtr->GetCharacterUnit().ToWeakPtr();
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

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");
