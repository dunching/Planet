
#include "PlanetAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>

#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"
#include "TestCommand.h"
#include "GameplayTagsSubSystem.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "TalentAllocationComponent.h"
#include "SceneUnitContainer.h"
#include "PlanetGameMode.h"

APlanetAIController::APlanetAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void APlanetAIController::OnPossess(APawn* InPawn)
{
	bool bIsNewPawn = (InPawn && InPawn != GetPawn());

	if (bIsNewPawn)
	{
	}

	Super::OnPossess(InPawn);

	if (bIsNewPawn)
	{
		if (InPawn)
		{
			BindPCWithCharacter();
		}
	}
}

UPlanetAbilitySystemComponent* APlanetAIController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetAbilitySystemComponent();
}

UGroupMnaggerComponent* APlanetAIController::GetGroupMnaggerComponent() const
{
	return GetPawn<FPawnType>()->GetGroupMnaggerComponent();
}

UHoldingItemsComponent* APlanetAIController::GetHoldingItemsComponent() const
{
	return GetPawn<FPawnType>()->GetHoldingItemsComponent();
}

UCharacterAttributesComponent* APlanetAIController::GetCharacterAttributesComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAttributesComponent();
}

UTalentAllocationComponent* APlanetAIController::GetTalentAllocationComponent() const
{
	return GetPawn<FPawnType>()->GetTalentAllocationComponent();
}

TSharedPtr<FCharacterProxy> APlanetAIController::GetCharacterUnit()
{
	return GetPawn<FPawnType>()->GetCharacterUnit();
}

ACharacterBase* APlanetAIController::GetRealCharacter()const
{
	return Cast<ACharacterBase>(GetPawn());
}

void APlanetAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APlanetAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

TWeakObjectPtr<ACharacterBase> APlanetAIController::GetTeamFocusTarget() const
{
	if (GetGroupMnaggerComponent() && GetGroupMnaggerComponent()->GetTeamHelper())
	{
		return GetGroupMnaggerComponent()->GetTeamHelper()->GetKnowCharacter();
	}

	return nullptr;
}

bool APlanetAIController::CheckIsFarawayOriginal() const
{
	return false;
}

void APlanetAIController::ResetGroupmateUnit(FCharacterProxy* NewGourpMateUnitPtr)
{
}

void APlanetAIController::BindPCWithCharacter()
{
}

TSharedPtr<FCharacterProxy> APlanetAIController::InitialCharacterUnit(ACharacterBase* CharaterPtr)
{
	return CharaterPtr->GetCharacterUnit();
}

void APlanetAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	auto CharacterPtr = Cast<AHumanCharacter>(Actor);
	if (CharacterPtr)
	{
		if (IsGroupmate(CharacterPtr))
		{
			return;
		}
		else
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				GetGroupMnaggerComponent()->GetTeamHelper()->AddKnowCharacter(CharacterPtr);
			}
			else
			{
				GetGroupMnaggerComponent()->GetTeamHelper()->RemoveKnowCharacter(CharacterPtr);
			}
		}
	}
}

void APlanetAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
}

bool APlanetAIController::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	return GetPawn<FPawnType>()->IsGroupmate(TargetCharacterPtr);
}

bool APlanetAIController::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return GetPawn<FPawnType>()->IsTeammate(TargetCharacterPtr);
}
