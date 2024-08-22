
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
		InitialCharacterUnit(Cast<ACharacterBase>(InPawn));
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

UCharacterUnit* APlanetAIController::GetCharacterUnit()
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

void APlanetAIController::ResetGroupmateUnit(UCharacterUnit* NewGourpMateUnitPtr)
{
}

void APlanetAIController::BindPCWithCharacter()
{
}

UCharacterUnit* APlanetAIController::InitialCharacterUnit(ACharacterBase* CharaterPtr)
{
	if (!CharaterPtr->GetCharacterUnit())
	{
		const auto CharacterUnitPtr =
			Cast<APlanetGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->AddCharacterUnit(RowName);

		CharaterPtr->SetCharacterUnit(CharacterUnitPtr);
		return CharacterUnitPtr;
	}

	return nullptr;
}
