#include "HumanCharacter_Player.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GravitySpringArmComponent.h"

#include "GeneratorNPCs_Patrol.h"
#include "PlayerComponent.h"
#include "GroupSharedInfo.h"
#include "GuideSubSystem.h"
#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "MainHUD.h"
#include "PlanetPlayerController.h"
#include "TaskNode.h"
#include "UICommon.h"
#include "InteractionList.h"
#include "MainHUDLayout.h"
#include "SceneActor.h"
#include "HumanInteractionWithNPC.h"

namespace HumanProcessor
{
	class FHumanInteractionWithNPCProcessor;
}

void UPlayerConversationComponent::DisplaySentence_Implementation(const FTaskNode_Conversation_SentenceInfo& Sentence)
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		OnPlayerHaveNewSentence.Broadcast(true, Sentence);
	}
#endif
}

void UPlayerConversationComponent::CloseConversationborder_Implementation()
{
#if UE_EDITOR || UE_CLIENT
	if (GetOwnerRole() < ROLE_Authority)
	{
		OnPlayerHaveNewSentence.Broadcast(false, FTaskNode_Conversation_SentenceInfo());
	}
#endif
}

AHumanCharacter_Player::AHumanCharacter_Player(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UPlayerConversationComponent>(
			UPlayerConversationComponent::ComponentName)
	)
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<UGravitySpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PlayerComponentPtr = CreateDefaultSubobject<UPlayerComponent>(UPlayerComponent::ComponentName);
}

void AHumanCharacter_Player::BeginPlay()
{
	Super::BeginPlay();
}

void AHumanCharacter_Player::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto GroupsHelperSPtr = GetGroupSharedInfo()->GetTeamMatesHelperComponent();
		if (GroupsHelperSPtr)
		{
			TeamMembersChangedDelegateHandle = GroupsHelperSPtr->MembersChanged.AddCallback(
				std::bind(&ThisClass::OnCharacterGroupMateChanged, this, std::placeholders::_1, std::placeholders::_2)
			);
		}
	}
#endif
}

void AHumanCharacter_Player::UnPossessed()
{
	Super::UnPossessed();
}

void AHumanCharacter_Player::OnRep_GroupSharedInfoChanged()
{
	Super::OnRep_GroupSharedInfoChanged();

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// 显示
		Cast<AMainHUD>(GetController<APlanetPlayerController>()->MyHUD)->InitalHUD();

		// 在SetPawn之后调用
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>(
			[this](auto NewProcessor)
			{
				NewProcessor->SetPawn(Cast<ThisClass>(this));
			});

		// 
		UGuideSubSystem::GetInstance()->InitializeMainLine();
		UGuideSubSystem::GetInstance()->ActiveMainLine();
	}
#endif
}

void AHumanCharacter_Player::InteractionSceneCharacter(AHumanCharacter_AI* CharacterPtr)
{
	Super::InteractionSceneCharacter(CharacterPtr);

	// 
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanInteractionWithNPCProcessor>();
}

void AHumanCharacter_Player::InitialGroupSharedInfo()
{
}

UCameraComponent* AHumanCharacter_Player::GetCameraComp()
{
	return FollowCamera;
}

USpringArmComponent* AHumanCharacter_Player::GetCameraBoom()
{
	return CameraBoom;
}

UPlayerConversationComponent* AHumanCharacter_Player::GetPlayerConversationComponent() const
{
	return Cast<UPlayerConversationComponent>(ConversationComponentPtr);
}

void AHumanCharacter_Player::UpdateSightActor()
{
	FVector OutCamLoc = FVector::ZeroVector;
	FRotator OutCamRot = FRotator::ZeroRotator;
	auto CameraManagerPtr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManagerPtr)
	{
		CameraManagerPtr->GetCameraViewPoint(OutCamLoc, OutCamRot);
	}

	auto StartPt = OutCamLoc;
	auto StopPt = OutCamLoc + (OutCamRot.Vector() * 1000);

	FHitResult Result;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);
	ObjectQueryParams.AddObjectTypesToQuery(ResouceBox_Object);

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);

	ISceneActorInteractionInterface* TempLookAtSceneObjPtr = nullptr;
	if (GetWorld()->LineTraceSingleByObjectType(
			Result,
			StartPt,
			StopPt,
			ObjectQueryParams,
			Params
		)
	)
	{
		if (Cast<ISceneActorInteractionInterface>(Result.GetActor()))
		{
			TempLookAtSceneObjPtr = Cast<ISceneActorInteractionInterface>(Result.GetActor());
		}
	}

	if (TempLookAtSceneObjPtr)
	{
		if (LookAtSceneActorPtr)
		{
			if (TempLookAtSceneObjPtr == LookAtSceneActorPtr)
			{
			}
			else
			{
				StartLookAt(TempLookAtSceneObjPtr);
			}
		}
		else
		{
			StartLookAt(TempLookAtSceneObjPtr);
		}
		LookingAt(TempLookAtSceneObjPtr);

		TempLookAtSceneObjPtr->HasBeenLookingAt(this);
	}
	else
	{
		EndLookAt();
		LookAtSceneActorPtr = nullptr;
	}
}

TPair<FVector, FVector> AHumanCharacter_Player::GetCharacterViewInfo()
{
	FMinimalViewInfo DesiredView;

	GetCameraComp()->GetCameraView(0, DesiredView);

	TPair<FVector, FVector> Result(
		DesiredView.Location, DesiredView.Location + (DesiredView.Rotation.Vector() * 1000)
	);

	return Result;
}

void AHumanCharacter_Player::StartLookAt(ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr)
{
	if (SceneActorInteractionInterfacePtr)
	{
		if (LookAtSceneActorPtr)
		{
			if (SceneActorInteractionInterfacePtr == LookAtSceneActorPtr)
			{
			}
			else
			{
				SceneActorInteractionInterfacePtr->HasBeenStartedLookAt(this);
				LookAtSceneActorPtr->HasBeenEndedLookAt();
				LookAtSceneActorPtr = SceneActorInteractionInterfacePtr;
			}
		}
		else
		{
			SceneActorInteractionInterfacePtr->HasBeenStartedLookAt(this);
			LookAtSceneActorPtr = SceneActorInteractionInterfacePtr;
		}
	}
	else
	{
		if (LookAtSceneActorPtr)
		{
			LookAtSceneActorPtr->HasBeenEndedLookAt();
		}

		LookAtSceneActorPtr = nullptr;
	}
}

void AHumanCharacter_Player::LookingAt(ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr)
{
	if (SceneActorInteractionInterfacePtr)
	{
		SceneActorInteractionInterfacePtr->HasBeenLookingAt(this);
	}
}

void AHumanCharacter_Player::EndLookAt()
{
	if (LookAtSceneActorPtr)
	{
		LookAtSceneActorPtr->HasBeenEndedLookAt();
	}

	if (InteractionListPtr)
	{
		InteractionListPtr->RemoveFromParent();
		InteractionListPtr = nullptr;
	}
}
