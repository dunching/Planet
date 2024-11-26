// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"

#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

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
#include "BaseFeatureComponent.h"
#include "ProxyProcessComponent.h"
#include "CDCaculatorComponent.h"

#include "CharacterTitle.h"
#include "UICommon.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "GAEvent_Helper.h"
#include "CharacterRisingTips.h"
#include "SceneObj.h"
#include "ItemProxy_Container.h"
#include "GroupSharedInfo.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	AbilitySystemComponentPtr = CreateDefaultSubobject<UPlanetAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponentPtr->SetIsReplicated(true);
	AbilitySystemComponentPtr->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CharacterAttributesComponentPtr = CreateDefaultSubobject<UCharacterAttributesComponent>(
		UCharacterAttributesComponent::ComponentName);
	TalentAllocationComponentPtr = CreateDefaultSubobject<UTalentAllocationComponent>(
		UTalentAllocationComponent::ComponentName);

	StateProcessorComponentPtr = CreateDefaultSubobject<UStateProcessorComponent>(
		UStateProcessorComponent::ComponentName);

	BaseFeatureComponentPtr = CreateDefaultSubobject<UBaseFeatureComponent>(UBaseFeatureComponent::ComponentName);
	ProxyProcessComponentPtr = CreateDefaultSubobject<UProxyProcessComponent>(UProxyProcessComponent::ComponentName);
	CDCaculatorComponentPtr = CreateDefaultSubobject<UCDCaculatorComponent>(UCDCaculatorComponent::ComponentName);
}

ACharacterBase::~ACharacterBase()
{
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// GA全部通过Server注册
		auto GASPtr = GetAbilitySystemComponent();

		GASPtr->ClearAllAbilities();
		GASPtr->InitAbilityActorInfo(this, this);
		GetBaseFeatureComponent()->InitialBaseGAs();
	}
#endif

	SwitchAnimLink(EAnimLinkClassType::kUnarmed);

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() < ROLE_Authority)
	{
		if (!CharacterTitlePtr)
		{
			auto AssetRefMapPtr = UAssetRefMap::GetInstance();
			CharacterTitlePtr = CreateWidget<UCharacterTitle>(GetWorldImp(), AssetRefMapPtr->AIHumanInfoClass);
			if (CharacterTitlePtr)
			{
				CharacterTitlePtr->CharacterPtr = this;
				if (GetLocalRole() == ROLE_AutonomousProxy)
				{
					CharacterTitlePtr->AddToViewport(EUIOrder::kPlayer_Character_State_HUD);
				}
				else
				{
					CharacterTitlePtr->AddToViewport(EUIOrder::kCharacter_State_HUD);
				}
			}
		}
	}
#endif

	auto CharacterAttributes = GetCharacterAttributesComponent()->GetCharacterAttributes();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 绑定一些会影响到 Character行动的数据
		HPChangedHandle = CharacterAttributes.HP.AddOnValueChanged(
			std::bind(&ThisClass::OnHPChanged, this, std::placeholders::_2)
		);

		MoveSpeedChangedHandle = CharacterAttributes.MoveSpeed.AddOnValueChanged(
			std::bind(&ThisClass::OnMoveSpeedChanged, this, std::placeholders::_2)
		);

		// 绑定 Character收到的效果回调(收到的是“最终结算效果”)
		ProcessedGAEventHandle = CharacterAttributes.ProcessedGAEvent.AddCallback(
			std::bind(&ThisClass::OnProcessedGAEVent, this, std::placeholders::_1)
		);
	}
#endif

	OnMoveSpeedChanged(CharacterAttributes.MoveSpeed.GetCurrentValue());
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
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
	}
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsA(APlanetPlayerController::StaticClass()))
	{
		GroupSharedInfoPtr = Cast<APlanetPlayerController>(NewController)->GroupSharedInfoPtr;
		
		GetHoldingItemsComponent()->InitialOwnerCharacterProxy(this);
	}
	else if (NewController->IsA(AHumanAIController::StaticClass()))
	{
	}

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

void ACharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
}

void ACharacterBase::InteractionSceneObj(ASceneObj* SceneObjPtr)
{
	InteractionSceneObj_Server(SceneObjPtr);
}

void ACharacterBase::Interaction(ACharacterBase* CharacterPtr)
{
}

void ACharacterBase::LookingAt(ACharacterBase* CharacterPtr)
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

AGroupSharedInfo* ACharacterBase::GetGroupSharedInfo() const
{
	return GroupSharedInfoPtr;
}

UHoldingItemsComponent* ACharacterBase::GetHoldingItemsComponent() const
{
	return GroupSharedInfoPtr ? GroupSharedInfoPtr->HoldingItemsComponentPtr : nullptr;
}

UCharacterAttributesComponent* ACharacterBase::GetCharacterAttributesComponent() const
{
	return CharacterAttributesComponentPtr;
}

UTalentAllocationComponent* ACharacterBase::GetTalentAllocationComponent() const
{
	return TalentAllocationComponentPtr;
}

UBaseFeatureComponent* ACharacterBase::GetBaseFeatureComponent() const
{
	return BaseFeatureComponentPtr;
}

UStateProcessorComponent* ACharacterBase::GetStateProcessorComponent() const
{
	return StateProcessorComponentPtr;
}

UProxyProcessComponent* ACharacterBase::GetProxyProcessComponent() const
{
	return ProxyProcessComponentPtr;
}

UCDCaculatorComponent* ACharacterBase::GetCDCaculatorComponent() const
{
	return CDCaculatorComponentPtr;
}

TSharedPtr<FCharacterProxy> ACharacterBase::GetCharacterUnit() const
{
	return GetHoldingItemsComponent()->CharacterProxySPtr;
}

void ACharacterBase::InteractionSceneObj_Server_Implementation(ASceneObj* SceneObjPtr)
{
	if (SceneObjPtr)
	{
		SceneObjPtr->Interaction(this);
	}
}

bool ACharacterBase::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	return GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(TargetCharacterPtr->GetCharacterUnit());
}

bool ACharacterBase::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return
		GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(TargetCharacterPtr->GetCharacterUnit()) ||
		TargetCharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(GetCharacterUnit());
}

ACharacterBase* ACharacterBase::GetFocusActor() const
{
	if (IsPlayerControlled())
	{
		auto PCPtr = GetController<APlanetPlayerController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(PCPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return TargetCharacterPtr;
		}
	}
	else
	{
		auto ACPtr = GetController<AAIController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(ACPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return TargetCharacterPtr;
		}
	}

	return nullptr;
}

void ACharacterBase::SwitchAnimLink_Client_Implementation(EAnimLinkClassType AnimLinkClassType)
{
	SwitchAnimLink(AnimLinkClassType);
}

void ACharacterBase::SetCampType_Implementation(ECharacterCampType CharacterCampType)
{
	if (CharacterTitlePtr)
	{
		CharacterTitlePtr->SetCampType(CharacterCampType);
	}
}

bool ACharacterBase::GetIsValidTarget() const
{
	TArray<FGameplayTag> Ary{
		UGameplayTagsSubSystem::DeathingTag,
		UGameplayTagsSubSystem::State_Buff_CantBeSlected
	};
	const auto TagContainer = FGameplayTagContainer::CreateFromArray(Ary);
	return !AbilitySystemComponentPtr->HasAnyMatchingGameplayTags(TagContainer);
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UInputProcessorSubSystem::GetInstance()->BindAction(InputComponent);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GroupSharedInfoPtr, COND_None);
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
		if (!GetController())
		{
			return;
		}
		if (IsPlayerControlled())
		{
			GetController<APlanetPlayerController>()->OnHPChanged(CurrentValue);
		}
		else
		{
			GetController<APlanetAIController>()->OnHPChanged(CurrentValue);
		}
	}
#endif
}

void ACharacterBase::OnMoveSpeedChanged_Implementation(int32 CurrentValue)
{
	GetCharacterMovement()->MaxWalkSpeed = CurrentValue;
	GetCharacterMovement()->MaxFlySpeed = CurrentValue;
}

void ACharacterBase::OnProcessedGAEVent_Implementation(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent)
{
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		// 显示对应的浮动UI
		auto UIPtr = CreateWidget<UCharacterRisingTips>(GetWorldImp(), UAssetRefMap::GetInstance()->FightingTipsClass);
		if (UIPtr->ProcessGAEVent(GAEvent))
		{
			UIPtr->AddToViewport(EUIOrder::kFightingTips);
		}
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
		}
		break;
	default:
		{
			
		};
	}
}

void ACharacterBase::OnRep_GroupSharedInfoChanged()
{
}