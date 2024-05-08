
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
		DelegateHandle = GroupsHelperSPtr->GroupsChanged.AddCallback(
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

void APlanetPlayerController::OnCharacterGroupMateChanged(EGroupMateChangeType GroupMateChangeType, ACharacterBase* NewCharacterPtr)
{
	APawn* CurrentPawn = GetPawn();

	switch (GroupMateChangeType)
	{
	case EGroupMateChangeType::kAdd:
	{
		if (NewCharacterPtr)
		{
			if (GetGroupMnaggerComponent()->GetGroupsHelper()->OwnerCharacterPtr == CurrentPawn)
			{
				auto AIPCPtr = Cast<APlanetAIController>(NewCharacterPtr->GetController());
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