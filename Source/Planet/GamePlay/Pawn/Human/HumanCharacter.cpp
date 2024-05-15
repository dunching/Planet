
#include "HumanCharacter.h"

#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "DrawDebugHelpers.h"
#include "Character/GravityMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

#include "ThreadSycAction.h"

#include "GameMode/PlanetGameMode.h"
#include "GenerateType.h"
#include <CacheAssetManager.h>
#include "Command/TestCommand.h"
#include "HumanActionPigInteractionUI.h"
#include "HumanViewBackpackProcessor.h"
#include "HumanRegularProcessor.h"
#include "ToolsLibrary.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "PlacingCellProcessor.h"
#include "ActionRoofBase.h"
#include "ActionStairBase.h"
#include "UIManagerSubSystem.h"
#include <ToolsMenu.h>
#include "Pawn/EquipmentElementComponent.h"
#include "Pawn/HoldingItemsComponent.h"
#include "Pawn/InputComponent/InputActions.h"
#include "InputProcessorSubSystem.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "HumanPlayerController.h"
#include "PlanetPlayerState.h"
#include "HumanAIController.h"
#include "AIHumanInfo.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"

AHumanCharacter::AHumanCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	GroupMnaggerComponentPtr = CreateDefaultSubobject<UGroupMnaggerComponent>(UGroupMnaggerComponent::ComponentName);
}

UGroupMnaggerComponent* AHumanCharacter::GetGroupMnaggerComponent()
{
	return GroupMnaggerComponentPtr;
}

TPair<FVector, FVector> AHumanCharacter::GetCharacterViewInfo()
{
	FMinimalViewInfo DesiredView;

	GetCameraComp()->GetCameraView(0, DesiredView);

	TPair<FVector, FVector>Result(
		DesiredView.Location, DesiredView.Location + (DesiredView.Rotation.Vector() * 1000)
	);

	return Result;
}

UGourpmateUnit* AHumanCharacter::GetGourpMateUnit()
{
	if (!GourpMateUnitPtr)
	{
		GourpMateUnitPtr = NewObject<UGourpmateUnit>();
		GourpMateUnitPtr->InitialByCharactor(this);
	}

	return GourpMateUnitPtr;
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetController()->IsA(AHumanPlayerController::StaticClass()))
	{
#if TESTHOLDDATA
		TestCommand::AddPlayerCharacterTestDataImp(this);
#endif
	}
	else if (GetController()->IsA(AHumanAIController::StaticClass()))
	{
#if TESTHOLDDATA
		TestCommand::AddAICharacterTestDataImp(this);
#endif
	}
}

void AHumanCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHumanCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AHumanCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsA(AHumanPlayerController::StaticClass()))
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>([this](auto NewProcessor) {
			NewProcessor->SetPawn(this);
			});
	} 
	else if (NewController->IsA(AHumanAIController::StaticClass()))
	{
		SwitchAnimLink(EAnimLinkClassType::kUnarmed);
	}
}
