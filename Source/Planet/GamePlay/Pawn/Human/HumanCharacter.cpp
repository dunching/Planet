
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



#include "GameMode/PlanetGameMode.h"
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
#include "InteractiveSkillComponent.h"
#include "HoldingItemsComponent.h"
#include "Pawn/InputComponent/InputActions.h"
#include "InputProcessorSubSystem.h"
#include "HorseProcessor.h"
#include "HumanProcessor.h"
#include "PlanetPlayerController.h"
#include "PlanetPlayerState.h"
#include "HumanAIController.h"
#include "CharacterTitle.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "GetItemInfos.h"

AHumanCharacter::AHumanCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	GroupMnaggerComponentPtr = CreateDefaultSubobject<UGroupMnaggerComponent>(UGroupMnaggerComponent::ComponentName);
}

UGroupMnaggerComponent* AHumanCharacter::GetGroupMnaggerComponent()
{
	return GroupMnaggerComponentPtr;
}

bool AHumanCharacter::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	if (TargetCharacterPtr == this)
	{
		return true;
	}

	auto GroupHelper = GroupMnaggerComponentPtr->GetGroupHelper();
	for (auto Iter : GroupHelper->MembersSet)
	{
		if (Iter == TargetCharacterPtr)
		{
			return true;
		}
	}

	if (GroupHelper->OwnerPtr == TargetCharacterPtr)
	{
		return true;
	}

	return false;
}

bool AHumanCharacter::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	if (TargetCharacterPtr == this)
	{
		return true;
	}

	if (IsGroupmate(TargetCharacterPtr))
	{
		auto TeammateHelper = GroupMnaggerComponentPtr->GetTeamHelper();
		for (auto Iter : TeammateHelper->MembersMap)
		{
			if (Iter.Value == TargetCharacterPtr)
			{
				return true;
			}
		}

		if (TeammateHelper->OwnerPtr == TargetCharacterPtr)
		{
			return true;
		}
	}

	return false;
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

	if (GetController())
	{
		if (GetController()->IsA(APlanetPlayerController::StaticClass()))
		{
			auto UIPtr = UUIManagerSubSystem::GetInstance()->GetItemInfos();

			OnSkillUnitChangedHanlde = GetHoldingItemsComponent()->GetHoldItemProperty().OnSkillUnitChanged.AddCallback(
				std::bind(&UGetItemInfos::OnSkillUnitChanged, UIPtr, std::placeholders::_1, std::placeholders::_2
				));

			OnConsumableUnitChangedHandle = GetHoldingItemsComponent()->GetHoldItemProperty().OnConsumableUnitChanged.AddCallback(
				std::bind(&UGetItemInfos::OnConsumableUnitChanged, UIPtr, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
				));

#if TESTPLAYERCHARACTERHOLDDATA
			TestCommand::AddPlayerCharacterTestDataImp(this);
#endif
		}
		else if (GetController()->IsA(AHumanAIController::StaticClass()))
		{
		}
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

void AHumanCharacter::OnCharacterGroupMateChanged(
	EGroupMateChangeType GroupMateChangeType,
	AHumanCharacter* LeaderPCPtr
)
{
	switch (GroupMateChangeType)
	{
	case EGroupMateChangeType::kAdd:
	{
		if (LeaderPCPtr)
		{
			if (LeaderPCPtr->GetGroupMnaggerComponent()->GetGroupHelper()->OwnerPtr == this)
			{
				auto AIPCPtr = LeaderPCPtr->GetController<AHumanAIController>();
				if (AIPCPtr)
				{
					AIPCPtr->SetCampType(ECharacterCampType::kTeamMate);
				}
			}
		}
	}
	break;
	}
}