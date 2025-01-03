
#include "HumanCharacter_Player.h"

#include "AssetRefMap.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GravitySpringArmComponent.h"

#include "GeneratorNPCs_Patrol.h"
#include "PlayerComponent.h"
#include "GroupSharedInfo.h"
#include "HumanAIController.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "MainHUD.h"
#include "PlanetPlayerController.h"

AHumanCharacter_Player::AHumanCharacter_Player(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<UGravitySpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PlayerComponentPtr = CreateDefaultSubobject<UPlayerComponent>(TEXT("PlayerComponent"));
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
	}
#endif
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

TPair<FVector, FVector> AHumanCharacter_Player::GetCharacterViewInfo()
{
	FMinimalViewInfo DesiredView;

	GetCameraComp()->GetCameraView(0, DesiredView);

	TPair<FVector, FVector>Result(
		DesiredView.Location, DesiredView.Location + (DesiredView.Rotation.Vector() * 1000)
	);

	return Result;
}
