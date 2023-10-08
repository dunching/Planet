
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

#include "GameMode/ShiYuGameMode.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "GenerateType.h"
#include "AnimInstanceHuman.h"
#include <CacheAssetManager.h>
#include "Command/TestCommand.h"
#include "SceneObj/CollisionDataStruct.h"
#include "SceneObj/Equipment/EquipmentBase.h"
#include "SceneObj/Building/BuildingBase.h"
#include "ActionProcess/HumanActionPigInteractionUI.h"
#include "ActionProcess/HumanViewBackpackProcessor.h"
#include "ActionProcess/HumanRegularProcessor.h"
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
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "GravitySpringComponent.h"

AHumanCharacter::AHumanCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.
		SetDefaultSubobjectClass<UHumanEquipmentSocksComponent>(UHumanEquipmentSocksComponent::ComponentName)
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

UCameraComponent* AHumanCharacter::GetCameraComp()
{
	return CameraComponentPtr;
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

void AHumanCharacter::BeginPlay()
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

#if TESTHOLDDATA
	TestCommand::AddCharacterTestDataImp(this);
#endif
}

void AHumanCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void UHumanEquipmentSocksComponent::UseItem(const FItemNum& Item)
{
	Super::UseItem(Item);

	if (CheckItemNumIsValid(Item))
	{
		switch (Item.ItemType.Enumtype)
		{
		case EEnumtype::kEquipment:
		{
			auto CacheAssetManagerPtr = UCacheAssetManager::GetInstance();
			if (!CacheAssetManagerPtr)
			{
				return;
			}
			auto ActorPtr = GetWorld()->SpawnActor<AEquipmentBase>(CacheAssetManagerPtr->GetClassByItemType(Item));
			if (!ActorPtr)
			{
				return;
			}

			ActorPtr->EquipItemToCharacter(Cast<AHumanCharacter>(GetOwner()));
			ItemsActionMap.Add(ActorPtr->GetPropertyComponent<UEquipmentInteractionComponent>()->GetItemSocketType(), ActorPtr);
		}
		break;
		case EEnumtype::kBuilding:
		{
			switch (std::get<EBuildingType>(Item.ItemType.ItemType))
			{
			case EBuildingType::kWoodCell:
			{
				UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HumanProcessor::FPlacingCellProcessor>([&](auto ActionProcessPtr)
					{
						if (ActionProcessPtr)
						{
							ActionProcessPtr->SetHoldItemsData(
								GetOwner<AHumanCharacter>()->GetHoldItemComponent()->GetHoldItemProperty()
							);
							ActionProcessPtr->SetPlaceBuildItem(Item);
						}
					}
				);
			}
			break;
			case EBuildingType::kWoodWall:
			case EBuildingType::kStoneWall:
			{
				UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HumanProcessor::FPlacingWallProcessor>([&](auto ActionProcessPtr)
					{
						if (ActionProcessPtr)
						{
							ActionProcessPtr->SetHoldItemsData(
								GetOwner<AHumanCharacter>()->GetHoldItemComponent()->GetHoldItemProperty()
							);
							ActionProcessPtr->SetPlaceBuildItem(Item);
						}
					}
				);
			}
			break;
			case EBuildingType::kWoodGroundStyle1:
			case EBuildingType::kWoodGroundStyle2:
			case EBuildingType::kStoneGround:
			{
				UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HumanProcessor::FPlacingGroundProcessor>([&](auto ActionProcessPtr)
					{
						if (ActionProcessPtr)
						{
							ActionProcessPtr->SetHoldItemsData(
								GetOwner<AHumanCharacter>()->GetHoldItemComponent()->GetHoldItemProperty()
							);
							ActionProcessPtr->SetPlaceBuildItem(Item);
						}
					}
				);
			}
			break;
			case EBuildingType::kWoodRoof:
			{
				UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HumanProcessor::FActionRoofPlace>([&](auto ActionBasePtr)
					{
					}
				);
			}
			break;
			case EBuildingType::kWoodStair:
			{
				UInputProcessorSubSystem::GetInstance()->SwitchActionProcess<HumanProcessor::FActionStairPlace>([&](auto ActionBasePtr)
					{
					}
				);
			}
			break;
			default:
			{
				check(0);
			}
			}
		}
		break;
		case EEnumtype::kRawMaterialType:
			break;
		}
	}
}

void AHumanCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}
