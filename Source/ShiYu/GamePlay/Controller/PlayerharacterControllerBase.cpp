
#include "PlayerharacterControllerBase.h"

#include "GameFramework/PlayerState.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

#include "InputProcessorSubSystem.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"

APlayerCharacterControllerBase::APlayerCharacterControllerBase(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void APlayerCharacterControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacterControllerBase::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

 void APlayerCharacterControllerBase::UpdateRotation(float DeltaTime)
 {
	 Super::UpdateRotation(DeltaTime);
}

 void APlayerCharacterControllerBase::OnPossess(APawn* PawnToPossess)
 {
	 Super::OnPossess(PawnToPossess);

	 if (PawnToPossess->IsA(AHumanCharacter::StaticClass()))
	 {
		 UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HumanProcessor::FHumanRegularProcessor>();
	 }
	 else if (PawnToPossess->IsA(AHorseCharacter::StaticClass()))
	 {
		 UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HorseProcessor::FHorseRegularProcessor>();
	 }
 }
