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
#include "TeamMatesHelperComponent.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "PlanetControllerInterface.h"
#include "GameplayTagsLibrary.h"
#include "StateProcessorComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "ProxyProcessComponent.h"
#include "CDCaculatorComponent.h"

#include "CharacterTitle.h"
#include "UICommon.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "GAEvent_Helper.h"
#include "CharacterRisingTips.h"
#include "SceneObj.h"
#include "GE_CharacterInitail.h"
#include "GroupSharedInfo.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	CharacterAttributesComponentPtr = CreateDefaultSubobject<UCharacterAttributesComponent>(
		UCharacterAttributesComponent::ComponentName
	);
	// CharacterAttributesComponentPtr->CharacterAttributeSetPtr = ;

	AbilitySystemComponentPtr = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(
		UCharacterAbilitySystemComponent::ComponentName
	);
	AbilitySystemComponentPtr->SetIsReplicated(true);
	AbilitySystemComponentPtr->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponentPtr->AddSpawnedAttribute(CreateDefaultSubobject<UAS_Character>(
		TEXT("CharacterAttributes1")
		));

	TalentAllocationComponentPtr = CreateDefaultSubobject<UTalentAllocationComponent>(
		UTalentAllocationComponent::ComponentName);

	StateProcessorComponentPtr = CreateDefaultSubobject<UStateProcessorComponent>(
		UStateProcessorComponent::ComponentName);

	ProxyProcessComponentPtr = CreateDefaultSubobject<UProxyProcessComponent>(UProxyProcessComponent::ComponentName);
	CDCaculatorComponentPtr = CreateDefaultSubobject<UCDCaculatorComponent>(UCDCaculatorComponent::ComponentName);
}

ACharacterBase::~ACharacterBase()
{
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SwitchAnimLink(EAnimLinkClassType::kUnarmed);

	auto CharacterAttributeSetPtr = GetCharacterAttributesComponent()->GetCharacterAttributes();

	// 绑定一些会影响到 Character行动的数据

	GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
		CharacterAttributeSetPtr->GetHPAttribute()
		).AddUObject(this, &ThisClass::OnHPChanged);

	GetCharacterAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
		CharacterAttributeSetPtr->GetMoveSpeedAttribute()
		).AddUObject(this, &ThisClass::OnMoveSpeedChanged);
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		// 绑定 Character收到的效果回调(收到的是“最终结算效果”)
		// ProcessedGAEventHandle = CharacterAttributes.ProcessedGAEvent.AddCallback(
		// 	std::bind(&ThisClass::OnProcessedGAEVent, this, std::placeholders::_1)
		// );
	}
#endif

	OnMoveSpeedChangedImp(CharacterAttributeSetPtr->GetMoveSpeed());
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
		GetCharacterAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	}
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsA(APlanetPlayerController::StaticClass()))
	{
		GroupSharedInfoPtr = Cast<APlanetPlayerController>(NewController)->GroupSharedInfoPtr;
		OnGroupSharedInfoReady(GroupSharedInfoPtr);
	}
	else if (NewController->IsA(AHumanAIController::StaticClass()))
	{
	}
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
	return GroupSharedInfoPtr ? GroupSharedInfoPtr->GetHoldingItemsComponent() : nullptr;
}

UCharacterAttributesComponent* ACharacterBase::GetCharacterAttributesComponent() const
{
	return CharacterAttributesComponentPtr;
}

UTalentAllocationComponent* ACharacterBase::GetTalentAllocationComponent() const
{
	return TalentAllocationComponentPtr;
}

UCharacterAbilitySystemComponent* ACharacterBase::GetCharacterAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
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

TSharedPtr<FCharacterProxy> ACharacterBase::GetCharacterProxy() const
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
	return
		GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(TargetCharacterPtr->GetCharacterProxy());
}

bool ACharacterBase::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return
		GetGroupSharedInfo()->GetTeamMatesHelperComponent()->IsMember(TargetCharacterPtr->GetCharacterProxy());
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
		UGameplayTagsLibrary::DeathingTag,
		UGameplayTagsLibrary::State_Buff_CantBeSlected
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

void ACharacterBase::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
	OnInitaliedGroupSharedInfo();
	
	auto NewController = GetController();

	GetHoldingItemsComponent()->InitialOwnerCharacterProxy(this);

	ForEachComponent(false, [this](UActorComponent* ComponentPtr)
	{
		auto GroupSharedInterfacePtr = Cast<IGroupSharedInterface>(ComponentPtr);
		if (GroupSharedInterfacePtr)
		{
			GroupSharedInterfacePtr->OnGroupSharedInfoReady(GroupSharedInfoPtr);
		}
	});
}

void ACharacterBase::OnMoveSpeedChanged(const FOnAttributeChangeData& CurrentValue)
{
	OnMoveSpeedChangedImp(CurrentValue.NewValue);
}

void ACharacterBase::OnMoveSpeedChangedImp(float Value)
{
	GetCharacterMovement()->MaxWalkSpeed = Value;
	GetCharacterMovement()->MaxFlySpeed = Value;
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
	const TSharedPtr<FCharacterProxyType>& TargetCharacterProxyPtr
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

void ACharacterBase::OnHPChanged(const FOnAttributeChangeData& CurrentValue)
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
			GetController<APlanetPlayerController>()->OnHPChanged(CurrentValue.NewValue);
		}
		else
		{
			GetController<APlanetAIController>()->OnHPChanged(CurrentValue.NewValue);
		}
	}
#endif
}

void ACharacterBase::OnRep_GroupSharedInfoChanged()
{
	OnGroupSharedInfoReady(GroupSharedInfoPtr);

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
}
