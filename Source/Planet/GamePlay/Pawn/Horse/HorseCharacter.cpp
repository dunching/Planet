
#include "HorseCharacter.h"

#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
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
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsSubSystem.h"

#include "GameMode_Main.h"
#include "PlanetPlayerState.h"
#include "GenerateType.h"
#include "Command/TestCommand.h"
#include "HumanActionPigInteractionUI.h"
#include "HorseViewBackpackProcessor.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"
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
#include "UnitProxyProcessComponent.h"
#include "HoldingItemsComponent.h"
#include "InputActions.h"
#include "InputProcessorSubSystem.h"
#include "PlanetPlayerController.h"
#include "HumanCharacter.h"
#include "BasicFutures_Mount.h"

AHorseCharacter::AHorseCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	MountTipsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MountTipsWidget"));
	MountTipsWidget->SetupAttachment(RootComponent);

	LeftMountCheckVolumetric = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftMountCheckVolumetric"));
	LeftMountCheckVolumetric->SetupAttachment(RootComponent);

	RightMountCheckVolumetric = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightMountCheckVolumetric"));
	RightMountCheckVolumetric->SetupAttachment(RootComponent);
}

TPair<FVector, FVector> AHorseCharacter::GetCharacterViewInfo()
{
	FMinimalViewInfo DesiredView;

	GetCameraComp()->GetCameraView(0, DesiredView);

	TPair<FVector, FVector>Result(
		DesiredView.Location, DesiredView.Location + (DesiredView.Rotation.Vector() * 1000)
	);

	return Result;
}

void AHorseCharacter::SwitchDisplayMountTips(bool bIsDisplay)
{
	if (MountTipsWidget)
	{
		MountTipsWidget->SetVisibility(bIsDisplay);
	}
}

void AHorseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (MountTipsWidget)
	{
		MountTipsWidget->SetVisibility(false);
	}
}

void AHorseCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AHorseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsA(APlanetPlayerController::StaticClass()))
	{
	}
}

void AHorseCharacter::Interaction(ACharacterBase* CharacterPtr)
{
	if (CharacterPtr)
	{
		FGameplayEventData Payload;
		auto GameplayAbilityTargetData_DashPtr = new FGameplayAbilityTargetData_Mount;

		GameplayAbilityTargetData_DashPtr->HorseCharacterPtr = this;

		Payload.TargetData.Add(GameplayAbilityTargetData_DashPtr);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterPtr, UGameplayTagsSubSystem::GetInstance()->Mount, Payload);
	}
}

void AHorseCharacter::StartLookAt(ACharacterBase* CharacterPtr)
{
	SwitchDisplayMountTips(true);
}

void AHorseCharacter::EndLookAt()
{
	SwitchDisplayMountTips(false);
}
