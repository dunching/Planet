
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



#include "GameMode_Main.h"
#include "GenerateType.h"
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
#include "ProxyProcessComponent.h"
#include "HoldingItemsComponent.h"
#include "InputActions.h"
#include "InputProcessorSubSystem.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "PlanetPlayerController.h"
#include "PlanetPlayerState.h"
#include "HumanAIController.h"
#include "CharacterTitle.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "GetItemInfosList.h"

#include "SceneUnitContainer.h"

AHumanCharacter::AHumanCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		if (GetController())
		{
			if (GetController()->IsA(APlanetPlayerController::StaticClass()))
			{
			}
			else if (GetController()->IsA(AHumanAIController::StaticClass()))
			{
			}
		}
	}
#endif
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

	if (NewController->IsA(APlanetPlayerController::StaticClass()))
	{
		auto GroupsHelperSPtr = GetGroupMnaggerComponent()->GetGroupHelper();
		if (GroupsHelperSPtr)
		{
			TeamMembersChangedDelegateHandle = GroupsHelperSPtr->MembersChanged.AddCallback(
				std::bind(&ThisClass::OnCharacterGroupMateChanged, this, std::placeholders::_1, std::placeholders::_2)
			);
		}
	}
	else if (NewController->IsA(AHumanAIController::StaticClass()))
	{
	}
}

void AHumanCharacter::UnPossessed()
{
	if (TeamMembersChangedDelegateHandle)
	{
		TeamMembersChangedDelegateHandle->UnBindCallback();
	}

	Super::UnPossessed();
}
