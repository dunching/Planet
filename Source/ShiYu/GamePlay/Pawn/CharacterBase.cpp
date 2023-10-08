// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/ShiYuGameMode.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"

#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "SceneObj/Equipment/EquipmentBase.h"
#include "PlayerState/ShiYuPlayerState.h"
#include "Component/ItemInteractionComponent.h"
#include "PawnIteractionComponent.h"
#include "HoldItemComponent.h"
#include "GenerateType.h"
#include "InputProcessor.h"
#include <UI/Menu/EquipItems/EquipMenu.h>
#include "InputComponent/ZYInputComponent.h"
#include "InputProcessorSubSystem.h"
#include "EquipmentSocksComponent.h"

UPawnIteractionComponent::UPawnIteractionComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

bool UPawnIteractionComponent::Attack(int32 Val)
{
	HP -= Val;

	if (HP <= 0)
	{
		GetOwner()->Destroy();

		return true;
	}
	return false;
}

void UPawnIteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

FName UPawnIteractionComponent::ComponentName = TEXT("PawnIteractionComponent");

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	InputComponentPtr = CreateDefaultSubobject<UZYInputComponent>(UZYInputComponent::ComponentName);

	PawnPropertyComponentPtr = CreateDefaultSubobject<UPawnIteractionComponent>(UPawnIteractionComponent::ComponentName);
	EquipmentSocksComponentPtr = CreateDefaultSubobject<UEquipmentSocksComponent>(UEquipmentSocksComponent::ComponentName);
	HoldItemComponentPtr = CreateDefaultSubobject<UHoldItemComponent>(UHoldItemComponent::ComponentName);
}

ACharacterBase::~ACharacterBase()
{

}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	InputComponentPtr->InitialInputMapping();
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AddMappingContext();
}


void ACharacterBase::AddMappingContext()
{
	auto PlayerControllerPtr = Cast<APlayerController>(Controller);
	if (PlayerControllerPtr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerControllerPtr->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputComponentPtr->InputMappingContext, 0);
		}

		FInputModeGameOnly InputMode;
// 		InputMode.SetHideCursorDuringCapture(true);
// 		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerControllerPtr->SetInputMode(InputMode);

// 		PlayerControllerPtr->bShowMouseCursor = true;
// 		PlayerControllerPtr->CurrentMouseCursor = EMouseCursor::Hand;
	}
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UInputProcessorSubSystem::GetInstance()->BindAction(InputComponentPtr, InputComponent);
}
