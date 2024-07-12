// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/PlanetGameMode.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"

#include "ToolsLibrary.h"
#include "Animation/AnimInstanceBase.h"
#include "Component/ItemInteractionComponent.h"
#include "HoldingItemsComponent.h"
#include "GenerateType.h"
#include "InputProcessor.h"
#include <ToolsMenu.h>
#include "InputComponent/InputActions.h"
#include "InputProcessorSubSystem.h"
#include "CharacterAttributesComponent.h"
#include "PlanetPlayerState.h"
#include "PlanetGameplayAbility.h"
#include "TestCommand.h"
#include "BasicFuturesBase.h"
#include "TalentAllocationComponent.h"
#include "GroupMnaggerComponent.h"
#include "AssetRefMap.h"
#include "HumanControllerInterface.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"
#include "InteractiveConsumablesComponent.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	AbilitySystemComponentPtr = CreateDefaultSubobject<UPlanetAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponentPtr->SetIsReplicated(true);
	AbilitySystemComponentPtr->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CharacterAttributesComponentPtr = CreateDefaultSubobject<UCharacterAttributesComponent>(UCharacterAttributesComponent::ComponentName);
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
	TalentAllocationComponentPtr = CreateDefaultSubobject<UTalentAllocationComponent>(UTalentAllocationComponent::ComponentName);

	InteractiveBaseGAComponentPtr = CreateDefaultSubobject<UInteractiveBaseGAComponent>(UInteractiveBaseGAComponent::ComponentName);
	InteractiveConsumablesComponentPtr = CreateDefaultSubobject<UInteractiveConsumablesComponent>(UInteractiveConsumablesComponent::ComponentName);
	InteractiveSkillComponentPtr = CreateDefaultSubobject<UInteractiveSkillComponent>(UInteractiveSkillComponent::ComponentName);
	InteractiveToolComponentPtr = CreateDefaultSubobject<UInteractiveToolComponent>(UInteractiveToolComponent::ComponentName);
}

ACharacterBase::~ACharacterBase()
{

}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SwitchAnimLink(EAnimLinkClassType::kUnarmed);
}

void ACharacterBase::Destroyed()
{
	Super::Destroyed();
}

void ACharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HPChangedHandle)
	{
		HPChangedHandle->UnBindCallback();
	}

	if (MoveSpeedChangedHandle)
	{
		MoveSpeedChangedHandle->UnBindCallback();
	}
	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		GASPtr->ClearAllAbilities();
		GASPtr->InitAbilityActorInfo(OnwerActorPtr, OnwerActorPtr);
	}
	GetInteractiveSkillComponent()->InitialBaseGAs();
	GetInteractiveBaseGAComponent()->InitialBaseGAs();
	GetInteractiveConsumablesComponent()->InitialBaseGAs();

	auto& CharacterAttributesRef = GetCharacterAttributesComponent()->GetCharacterAttributes();
	OnMoveSpeedChanged(CharacterAttributesRef.MoveSpeed.GetCurrentValue());

	HPChangedHandle = CharacterAttributesRef.HP.AddOnValueChanged(
		std::bind(&ThisClass::OnHPChanged, this, std::placeholders::_2)
	);
	MoveSpeedChangedHandle = CharacterAttributesRef.MoveSpeed.AddOnValueChanged(
		std::bind(&ThisClass::OnMoveSpeedChanged, this, std::placeholders::_2)
	);
}

void ACharacterBase::Interaction(ACharacterBase* CharacterPtr)
{

}

void ACharacterBase::StartLookAt(ACharacterBase* CharacterPtr)
{

}

void ACharacterBase::EndLookAt()
{

}

class UPlanetAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
}

UHoldingItemsComponent* ACharacterBase::GetHoldingItemsComponent()
{
	return HoldingItemsComponentPtr;
}

UCharacterAttributesComponent* ACharacterBase::GetCharacterAttributesComponent()
{
	return CharacterAttributesComponentPtr;
}

UTalentAllocationComponent* ACharacterBase::GetTalentAllocationComponent()
{
	return TalentAllocationComponentPtr;
}

UInteractiveBaseGAComponent* ACharacterBase::GetInteractiveBaseGAComponent()
{
	return InteractiveBaseGAComponentPtr;
}

UInteractiveConsumablesComponent* ACharacterBase::GetInteractiveConsumablesComponent()
{
	return InteractiveConsumablesComponentPtr;
}

UInteractiveSkillComponent* ACharacterBase::GetInteractiveSkillComponent()
{
	return InteractiveSkillComponentPtr;
}

UInteractiveToolComponent* ACharacterBase::GetInteractiveToolComponent()
{
	return InteractiveToolComponentPtr;
}

bool ACharacterBase::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	return false;
}

bool ACharacterBase::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return false;
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UInputProcessorSubSystem::GetInstance()->BindAction(InputComponent);
}

void ACharacterBase::OnHPChanged(int32 CurrentValue)
{
	if (CurrentValue <= 0)
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer{ UGameplayTagsSubSystem::GetInstance()->DeathingTag });
		GetAbilitySystemComponent()->OnAbilityEnded.AddLambda([this](const FAbilityEndedData& AbilityEndedData) {
			for (auto Iter : AbilityEndedData.AbilityThatEnded->AbilityTags)
			{
				if (Iter == UGameplayTagsSubSystem::GetInstance()->DeathingTag)
				{
					Destroy();
				}
			}
			});
	}
}

void ACharacterBase::OnMoveSpeedChanged(int32 CurrentValue)
{
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterAttributesComponent()->GetCharacterAttributes().MoveSpeed.GetCurrentValue();
}
