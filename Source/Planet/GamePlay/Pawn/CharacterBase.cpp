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
#include "InputActions.h"
#include "InputProcessorSubSystem.h"
#include "CharacterAttributesComponent.h"
#include "PlanetPlayerState.h"
#include "PlanetGameplayAbility.h"
#include "TestCommand.h"
#include "BasicFuturesBase.h"
#include "TalentAllocationComponent.h"
#include "GroupMnaggerComponent.h"
#include "AssetRefMap.h"
#include "PlanetControllerInterface.h"
#include "GameplayTagsSubSystem.h"
#include "StateProcessorComponent.h"
#include "InteractiveBaseGAComponent.h"
#include "InteractiveConsumablesComponent.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "CharacterTitle.h"
#include "UICommon.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "GAEvent_Helper.h"
#include "FightingTips.h"

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
	GroupMnaggerComponentPtr = CreateDefaultSubobject<UGroupMnaggerComponent>(UGroupMnaggerComponent::ComponentName);

	StateProcessorComponentPtr = CreateDefaultSubobject<UStateProcessorComponent>(UStateProcessorComponent::ComponentName);

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

	auto  asd = GetNetMode();

	SwitchAnimLink(EAnimLinkClassType::kUnarmed);

	auto AssetRefMapPtr = UAssetRefMap::GetInstance();

	if (!CharacterTitlePtr)
	{
		CharacterTitlePtr = CreateWidget<UCharacterTitle>(GetWorldImp(), AssetRefMapPtr->AIHumanInfoClass);
		if (CharacterTitlePtr)
		{
			CharacterTitlePtr->CharacterPtr = this;
			CharacterTitlePtr->AddToViewport(EUIOrder::kCharacter_State_HUD);
		}
	}

	auto CharacterAttributesSPtr = GetCharacterAttributesComponent()->GetCharacterAttributes();
	OnMoveSpeedChanged(CharacterAttributesSPtr->MoveSpeed.GetCurrentValue());

	HPChangedHandle = CharacterAttributesSPtr->HP.AddOnValueChanged(
		std::bind(&ThisClass::OnHPChanged, this, std::placeholders::_2)
	);

	MoveSpeedChangedHandle = CharacterAttributesSPtr->MoveSpeed.AddOnValueChanged(
		std::bind(&ThisClass::OnMoveSpeedChanged, this, std::placeholders::_2)
	);

	ProcessedGAEventHandle = CharacterAttributesSPtr->ProcessedGAEvent.AddCallback(
		std::bind(&ThisClass::OnProcessedGAEVent, this, std::placeholders::_1)
	);
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

	if (CharacterTitlePtr)
	{
		CharacterTitlePtr->RemoveFromParent();
		CharacterTitlePtr = nullptr;
	}

	OriginalAIController = nullptr;

	Super::EndPlay(EndPlayReason);
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::PostInitializeComponents()
{
	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		InitialCharacterUnit();

		auto GASPtr = GetAbilitySystemComponent();

		GASPtr->ClearAllAbilities();
		GASPtr->InitAbilityActorInfo(this, this);
		GetInteractiveSkillComponent()->InitialBaseGAs();
		GetInteractiveBaseGAComponent()->InitialBaseGAs();
		GetInteractiveConsumablesComponent()->InitialBaseGAs();
	}

	Super::PostInitializeComponents();
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ACharacterBase::UnPossessed()
{
	AController* const OldController = Controller;

	Super::UnPossessed();

	if (IsActorBeingDestroyed())
	{
//		SpawnDefaultController();
	}
	else if(OriginalAIController)
	{
		if (OldController)
		{
			if (OldController->IsA(AAIController::StaticClass()))
			{
				//		OldController->Destroy();
			}
			else if (OldController->IsA(APlayerController::StaticClass()))
			{
				OriginalAIController->Possess(this);
			}
		}
	}
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

UHoldingItemsComponent* ACharacterBase::GetHoldingItemsComponent()const
{
	return HoldingItemsComponentPtr;
}

UCharacterAttributesComponent* ACharacterBase::GetCharacterAttributesComponent()const
{
	return CharacterAttributesComponentPtr;
}

UTalentAllocationComponent* ACharacterBase::GetTalentAllocationComponent()const
{
	return TalentAllocationComponentPtr;
}

UInteractiveBaseGAComponent* ACharacterBase::GetInteractiveBaseGAComponent()const
{
	return InteractiveBaseGAComponentPtr;
}

UInteractiveConsumablesComponent* ACharacterBase::GetInteractiveConsumablesComponent()const
{
	return InteractiveConsumablesComponentPtr;
}

UInteractiveSkillComponent* ACharacterBase::GetInteractiveSkillComponent()const
{
	return InteractiveSkillComponentPtr;
}

UInteractiveToolComponent* ACharacterBase::GetInteractiveToolComponent()const
{
	return InteractiveToolComponentPtr;
}

UGroupMnaggerComponent* ACharacterBase::GetGroupMnaggerComponent()const
{
	return GroupMnaggerComponentPtr;
}

TSharedPtr<FCharacterProxy> ACharacterBase::GetCharacterUnit()const
{
	return CharacterUnitPtr;
}

void ACharacterBase::InitialCharacterUnit()
{
	CharacterUnitPtr = MakeShared<FCharacterProxy>();
	CharacterUnitPtr->ProxyCharacterPtr = this;
}

bool ACharacterBase::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	return GetGroupMnaggerComponent()->GetGroupHelper()->IsMember(TargetCharacterPtr->GetCharacterUnit());
}

bool ACharacterBase::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return GetGroupMnaggerComponent()->GetTeamHelper()->IsMember(TargetCharacterPtr->GetCharacterUnit());
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UInputProcessorSubSystem::GetInstance()->BindAction(InputComponent);
}

void ACharacterBase::SpawnDefaultController()
{
	Super::SpawnDefaultController();

	OriginalAIController = Controller;
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
	GetCharacterMovement()->MaxWalkSpeed = CurrentValue;
}

void ACharacterBase::OnProcessedGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
	// 显示对应的浮动UI
	auto UIPtr = CreateWidget<UFightingTips>(GetWorldImp(), FightingTipsClass);
	UIPtr->ProcessGAEVent(GAEvent);
	UIPtr->AddToViewport(EUIOrder::kFightingTips);
}

void ACharacterBase::OnCharacterGroupMateChanged(
	EGroupMateChangeType GroupMateChangeType,
	const TSharedPtr<FCharacterUnitType>& TargetCharacterUnitPtr
)
{
	switch (GroupMateChangeType)
	{
	case EGroupMateChangeType::kAdd:
	{
		if (TargetCharacterUnitPtr)
		{
			if (TargetCharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetGroupHelper()->OwnerCharacterUnitPtr == GetCharacterUnit())
			{
				auto AIPCPtr = TargetCharacterUnitPtr->ProxyCharacterPtr->GetController<AHumanAIController>();
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