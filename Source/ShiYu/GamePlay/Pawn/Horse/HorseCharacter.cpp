
#include "HorseCharacter.h"

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

#include "GameMode/ShiYuGameMode.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "GenerateType.h"
#include <AnimInstanceHuman.h>
#include <CacheAssetManager.h>
#include "Command/TestCommand.h"
#include "SceneObj/CollisionDataStruct.h"
#include "SceneObj/Equipment/EquipmentBase.h"
#include "SceneObj/Building/BuildingBase.h"
#include "ActionProcess/HumanActionPigInteractionUI.h"
#include "ActionProcess/HorseViewBackpackProcessor.h"
#include "ActionProcess/HumanRegularProcessor.h"
#include "ActionProcess/HorseRegularProcessor.h"
#include "ToolsLibrary.h"
#include "PlacingBuildingAreaProcessor.h"
#include "ActionTrackPlace.h" 
#include "ActionTrackVehiclePlace.h"
#include "PlacingWallProcessor.h"
#include "PlacingGroundProcessor.h"
#include "PlacingCellProcessor.h"
#include "ActionRoofBase.h"
#include "ActionStairBase.h"
#include "UI/UIManagerSubSystem.h"
#include <UI/Menu/EquipItems/EquipMenu.h>
#include "Pawn/PawnIteractionComponent.h"
#include "Pawn/EquipmentSocksComponent.h"
#include "Pawn/PawnDataStruct.h"
#include "Pawn/HoldItemComponent.h"
#include "Pawn/InputComponent/ZYInputComponent.h"
#include "InputProcessorSubSystem.h"
#include "GravitySpringComponent.h"

AHorseCharacter::AHorseCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.
		SetDefaultSubobjectClass<UHorseEquipmentSocksComponent>(UHorseEquipmentSocksComponent::ComponentName)
	)
{
	SpringArmComponentPtr = CreateDefaultSubobject<UGravitySpringComponent>(TEXT("SpringArmComponent"));
	SpringArmComponentPtr->SetupAttachment(RootComponent);
	SpringArmComponentPtr->TargetArmLength = 300.0f;
	SpringArmComponentPtr->bDoCollisionTest = 0;
	SpringArmComponentPtr->bEnableCameraLag = 1;
	SpringArmComponentPtr->bEnableCameraRotationLag = 1;

	CameraComponentPtr = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponentPtr->SetupAttachment(SpringArmComponentPtr);
}

UCameraComponent* AHorseCharacter::GetCameraComp()
{
	return CameraComponentPtr;
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

void AHorseCharacter::BeginPlay()
{
	Super::BeginPlay();

	auto UIManagerPtr = Cast<UUIManagerSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(this, UUIManagerSubSystem::StaticClass())
		);
	if (UIManagerPtr)
	{
		auto EquipUIPtr = UIManagerPtr->DisplayEquipMenuUI();
		if (EquipUIPtr)
		{
			GetHoldItemComponent()->GetHoldItemProperty()->SetItemChangeCB(
				EOnItemChangeNotityObjType::kEquipUI,
				std::bind(&UEquipMenu::ResetFiled, EquipUIPtr)
			);
		}
	}

	FItemNum HandleItemEquipment;
	HandleItemEquipment.ItemType.ItemType = EEquipmentType::kHandEquipment;
	HandleItemEquipment.ItemType.Enumtype = EEnumtype::kEquipment;
	HandleItemEquipment.Num = 1;
	EquipmentSocksComponentPtr->UseItem(HandleItemEquipment);
}

void AHorseCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void UHorseEquipmentSocksComponent::UseItem(const FItemNum& Item)
{
	Super::UseItem(Item);
}

void AHorseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}
