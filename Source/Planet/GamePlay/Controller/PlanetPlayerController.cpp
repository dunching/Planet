
#include "PlanetPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "EnhancedInputSubsystems.h"

#include "InputProcessorSubSystem.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"
#include "InputActions.h"
#include "UIManagerSubSystem.h"
#include "CharacterBase.h"
#include "HoldingItemsComponent.h"
#include "GroupMnaggerComponent.h"
#include "PlanetAIController.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "PlanetControllerInterface.h"
#include "NavgationSubSysetem.h"

APlanetPlayerController::APlanetPlayerController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void APlanetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(
			UInputProcessorSubSystem::GetInstance()->InputActionsPtr->InputMappingContext,
			0
		);
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	UNavgationSubSystem::GetInstance();
}

void APlanetPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void APlanetPlayerController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);
}

void APlanetPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto GroupsHelperSPtr = GetGroupMnaggerComponent()->GetGroupsHelper();
	if (GroupsHelperSPtr)
	{
		DelegateHandle = GroupsHelperSPtr->MembersChanged.AddCallback(
			std::bind(&ThisClass::OnCharacterGroupMateChanged, this, std::placeholders::_1, std::placeholders::_2)
		);
	}
}

void APlanetPlayerController::OnUnPossess()
{
	APawn* CurrentPawn = GetPawn();

	auto CharacterPtr = Cast<ACharacterBase>(CurrentPawn);
	if (CharacterPtr)
	{
		if (DelegateHandle)
		{
			DelegateHandle->UnBindCallback();
		}
	}

	Super::OnUnPossess();
}

bool APlanetPlayerController::InputKey(const FInputKeyParams& Params)
{
	auto Result = Super::InputKey(Params);

	UInputProcessorSubSystem::GetInstance()->InputKey(Params);

	return Result;
}

UGroupMnaggerComponent* APlanetPlayerController::GetGroupMnaggerComponent()
{
	if (!GroupMnaggerComponentPtr)
	{
		GroupMnaggerComponentPtr = Cast<UGroupMnaggerComponent>(AddComponentByClass(UGroupMnaggerComponent::StaticClass(), true, FTransform::Identity, false));
	}
	return  GroupMnaggerComponentPtr;
}

UGourpmateUnit* APlanetPlayerController::GetGourpMateUnit()
{
	return Cast<AHumanCharacter>(GetPawn())->GetGourpMateUnit();
}

void APlanetPlayerController::OnCharacterGroupMateChanged(
	EGroupMateChangeType GroupMateChangeType,
	IPlanetControllerInterface* NewPCPtr
)
{
	switch (GroupMateChangeType)
	{
	case EGroupMateChangeType::kAdd:
	{
		if (NewPCPtr)
		{
			if (GetGroupMnaggerComponent()->GetGroupsHelper()->OwnerPCPtr == this)
			{
				auto AIPCPtr = Cast<APlanetAIController>(NewPCPtr);
				if (AIPCPtr)
				{
					AIPCPtr->SetCampType(ECharacterCampType::kTeamMate);

					AIPCPtr->OnTeammateOptionChanged(
						GetGroupMnaggerComponent()->GetTeamsHelper()->GetTeammateOption(), Cast<ACharacter>(GetPawn())
					);
				}
			}
		}
	}
	break;
	}
}