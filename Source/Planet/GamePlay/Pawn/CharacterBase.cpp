// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"

#include "GameMode_Main.h"
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
#include "BaseFeatureGAComponent.h"

#include "UnitProxyProcessComponent.h"

#include "CharacterTitle.h"
#include "UICommon.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "GAEvent_Helper.h"
#include "FightingTips.h"
#include "SceneObj.h"
#include "SceneUnitContainer.h"
#include "SceneUnitTable.h"

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

	InteractiveBaseGAComponentPtr = CreateDefaultSubobject<UBaseFeatureGAComponent>(UBaseFeatureGAComponent::ComponentName);
	InteractiveSkillComponentPtr = CreateDefaultSubobject<UUnitProxyProcessComponent>(UUnitProxyProcessComponent::ComponentName);

}

ACharacterBase::~ACharacterBase()
{

}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SwitchAnimLink(EAnimLinkClassType::kUnarmed);

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (!CharacterTitlePtr)
		{
			auto AssetRefMapPtr = UAssetRefMap::GetInstance();
			CharacterTitlePtr = CreateWidget<UCharacterTitle>(GetWorldImp(), AssetRefMapPtr->AIHumanInfoClass);
			if (CharacterTitlePtr)
			{
				CharacterTitlePtr->CharacterPtr = this;
				CharacterTitlePtr->AddToViewport(EUIOrder::kCharacter_State_HUD);
			}
		}

		InitialCharacterUnitInClient();
	}
#endif

	auto CharacterAttributesSPtr = GetCharacterAttributesComponent()->GetCharacterAttributes();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
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
#endif

	OnMoveSpeedChanged(CharacterAttributesSPtr->MoveSpeed.GetCurrentValue());
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

#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			// GA全部通过Server注册
			auto GASPtr = GetAbilitySystemComponent();

			GASPtr->ClearAllAbilities();
			GASPtr->InitAbilityActorInfo(this, this);
			GetInteractiveBaseGAComponent()->InitialBaseGAs();
		}
#endif
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
	else if (OriginalAIController)
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

void ACharacterBase::InteractionSceneObj_Implementation(ASceneObj* SceneObjPtr)
{
	if (SceneObjPtr)
	{
		SceneObjPtr->Interaction(this);
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

UBaseFeatureGAComponent* ACharacterBase::GetInteractiveBaseGAComponent()const
{
	return InteractiveBaseGAComponentPtr;
}

UStateProcessorComponent* ACharacterBase::GetStateProcessorComponent() const
{
	return StateProcessorComponentPtr;
}

UUnitProxyProcessComponent* ACharacterBase::GetInteractiveSkillComponent()const
{
	return InteractiveSkillComponentPtr;
}

UGroupMnaggerComponent* ACharacterBase::GetGroupMnaggerComponent()const
{
	return GroupMnaggerComponentPtr;
}

TSharedPtr<FCharacterProxy> ACharacterBase::GetCharacterUnit()const
{
	return HoldingItemsComponentPtr->SceneUnitContainer->OwnerCharacter;
}

TSharedPtr<FSceneUnitContainer> ACharacterBase::GetSceneUnitContainer() const
{
	return HoldingItemsComponentPtr->SceneUnitContainer;
}

void ACharacterBase::InitialCharacterUnit()
{
	auto SceneUnitContainer = GetSceneUnitContainer();
	auto NewCharacterUnitPtr = GetSceneUnitContainer()->AddUnit_Character(RowName);
	NewCharacterUnitPtr->OwnerCharacterUnitPtr = nullptr;
	NewCharacterUnitPtr->ProxyCharacterPtr = this;
	
	// 
	SceneUnitContainer->OwnerCharacter = NewCharacterUnitPtr;
	
	// 
	GetInteractiveSkillComponent()->GetAllocationSkills()->OwnerCharacter = NewCharacterUnitPtr;

	// 
	auto TableRowUnit_CharacterInfoPtr = NewCharacterUnitPtr->GetTableRowUnit_CharacterInfo();
	*GetCharacterAttributesComponent()->CharacterAttributesSPtr = TableRowUnit_CharacterInfoPtr->CharacterAttributes;
}

void ACharacterBase::InitialCharacterUnitInClient_Implementation()
{
	auto NewCharacterUnitPtr = GetSceneUnitContainer()->AddUnit_Character(RowName);
	NewCharacterUnitPtr->OwnerCharacterUnitPtr = nullptr;
}

bool ACharacterBase::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	return GetGroupMnaggerComponent()->GetGroupHelper()->IsMember(TargetCharacterPtr->GetCharacterUnit());
}

bool ACharacterBase::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return GetGroupMnaggerComponent()->GetTeamHelper()->IsMember(TargetCharacterPtr->GetCharacterUnit());
}

void ACharacterBase::SwitchAnimLink_Client_Implementation(EAnimLinkClassType AnimLinkClassType)
{
	SwitchAnimLink(AnimLinkClassType);
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

void ACharacterBase::OnHPChanged_Implementation(int32 CurrentValue)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
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
#endif
}

void ACharacterBase::OnMoveSpeedChanged_Implementation(int32 CurrentValue)
{
	GetCharacterMovement()->MaxWalkSpeed = CurrentValue;
}

void ACharacterBase::OnProcessedGAEVent_Implementation(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		// 显示对应的浮动UI
		auto UIPtr = CreateWidget<UFightingTips>(GetWorldImp(), FightingTipsClass);
		UIPtr->ProcessGAEVent(GAEvent);
		UIPtr->AddToViewport(EUIOrder::kFightingTips);
	}
#endif
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