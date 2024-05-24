
#include "HumanPlayerController.h"

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
#include "HumanAIController.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HumanControllerInterface.h"
#include "NavgationSubSysetem.h"
#include "AssetRefMap.h"
#include "FocusIcon.h"
#include "TestCommand.h"

AHumanPlayerController::AHumanPlayerController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void AHumanPlayerController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	ClearFocus(InPriority);

	if (NewFocus)
	{
		if (InPriority >= FocusInformation.Priorities.Num())
		{
			FocusInformation.Priorities.SetNum(InPriority + 1);
		}
		FocusInformation.Priorities[InPriority].Actor = NewFocus;

		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		FocusIconPtr = CreateWidget<UFocusIcon>(GetWorldImp(), AssetRefMapPtr->FocusIconClass);
		if (FocusIconPtr)
		{
			FocusIconPtr->FocusItem = FocusInformation.Priorities[InPriority];
			FocusIconPtr->AddToViewport();
		}
	}
}

AActor* AHumanPlayerController::GetFocusActor() const
{
	AActor* FocusActor = nullptr;
	for (int32 Index = FocusInformation.Priorities.Num() - 1; Index >= 0; --Index)
	{
		const FFocusKnowledge::FFocusItem& FocusItem = FocusInformation.Priorities[Index];
		FocusActor = FocusItem.Actor.Get();
		if (FocusActor)
		{
			break;
		}
		else if (FAISystem::IsValidLocation(FocusItem.Position))
		{
			break;
		}
	}

	return FocusActor;
}

void AHumanPlayerController::ClearFocus(EAIFocusPriority::Type InPriority)
{
	if (InPriority < FocusInformation.Priorities.Num())
	{
		FocusInformation.Priorities[InPriority].Actor = nullptr;
		FocusInformation.Priorities[InPriority].Position = FAISystem::InvalidLocation;
	}

	if (FocusIconPtr)
	{
		FocusIconPtr->RemoveFromParent();
		FocusIconPtr = nullptr;
	}
}

void AHumanPlayerController::BeginPlay()
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

void AHumanPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AHumanPlayerController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);
}

void AHumanPlayerController::OnPossess(APawn* InPawn)
{
	bool bIsNewPawn = (InPawn && InPawn != GetPawn());

	Super::OnPossess(InPawn);

	if (bIsNewPawn)
	{
		if (InPawn && InPawn->IsA(AHumanCharacter::StaticClass()))
		{
#if TESTHOLDDATA
			TestCommand::AddPlayerCharacterTestDataImp(Cast<AHumanCharacter>(InPawn));
#endif

			auto GroupsHelperSPtr = GetGroupMnaggerComponent()->GetGroupHelper();
			if (GroupsHelperSPtr)
			{
				DelegateHandle = GroupsHelperSPtr->MembersChanged.AddCallback(
					std::bind(&ThisClass::OnCharacterGroupMateChanged, this, std::placeholders::_1, std::placeholders::_2)
				);
			}

			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>([this, InPawn](auto NewProcessor) {
				NewProcessor->SetPawn(Cast<AHumanCharacter>(InPawn));
				});
		}
	}
}

void AHumanPlayerController::OnUnPossess()
{
	APawn* CurrentPawn = GetPawn();

	auto CharacterPtr = Cast<FPawnType>(CurrentPawn);
	if (CharacterPtr)
	{
		if (DelegateHandle)
		{
			DelegateHandle->UnBindCallback();
		}
	}

	Super::OnUnPossess();
}

void AHumanPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
}

bool AHumanPlayerController::InputKey(const FInputKeyParams& Params)
{
	auto Result = Super::InputKey(Params);

	UInputProcessorSubSystem::GetInstance()->InputKey(Params);

	return Result;
}

UGroupMnaggerComponent* AHumanPlayerController::GetGroupMnaggerComponent()const
{
	return GetPawn<FPawnType>()->GetGroupMnaggerComponent();
}

UGourpmateUnit* AHumanPlayerController::GetGourpMateUnit()
{
	return GetPawn<FPawnType>()->GetGourpMateUnit();
}

void AHumanPlayerController::OnCharacterGroupMateChanged(
	EGroupMateChangeType GroupMateChangeType,
	FPawnType* LeaderPCPtr
)
{
	switch (GroupMateChangeType)
	{
	case EGroupMateChangeType::kAdd:
	{
		if (LeaderPCPtr)
		{
			if (LeaderPCPtr->GetGroupMnaggerComponent()->GetGroupHelper()->OwnerPCPtr == GetPawn<FPawnType>())
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