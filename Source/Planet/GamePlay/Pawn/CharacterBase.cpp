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
#include "Component/SceneActorInteractionComponent.h"
#include "InventoryComponent.h"
#include "GenerateType.h"
#include "InputProcessor.h"
#include "InputActions.h"
#include "InputProcessorSubSystem.h"
#include "CharacterAttributesComponent.h"
#include "PlanetPlayerState.h"
#include "PlanetGameplayAbility.h"
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
#include "ConversationComponent.h"

#include "CharacterTitle.h"
#include "UICommon.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "CharacterRisingTips.h"
#include "CharacterTitleComponent.h"
#include "SceneActor.h"
#include "GE_CharacterInitail.h"
#include "GroupManagger.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	if (GetMesh())
	{
		GetMesh()->SetHiddenInGame(true);

		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		
		CopyPoseMeshPtr = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("CopyMesh"));
		if (CopyPoseMeshPtr)
		{
			CopyPoseMeshPtr->AlwaysLoadOnClient = true;
			CopyPoseMeshPtr->AlwaysLoadOnServer = true;
			CopyPoseMeshPtr->bOwnerNoSee = false;
			CopyPoseMeshPtr->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
			CopyPoseMeshPtr->bCastDynamicShadow = true;
			CopyPoseMeshPtr->bAffectDynamicIndirectLighting = true;
			CopyPoseMeshPtr->PrimaryComponentTick.TickGroup = TG_PrePhysics;
			CopyPoseMeshPtr->SetupAttachment(GetMesh());
			CopyPoseMeshPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	CharacterAttributesComponentPtr = CreateDefaultSubobject<UCharacterAttributesComponent>(
		UCharacterAttributesComponent::ComponentName
	);
	// CharacterAttributesComponentPtr->CharacterAttributeSetPtr = ;

	AbilitySystemComponentPtr = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(
		UCharacterAbilitySystemComponent::ComponentName
	);
	AbilitySystemComponentPtr->AddSpawnedAttribute(CreateDefaultSubobject<UAS_Character>(
		TEXT("CharacterAttributes1")
	));

	TalentAllocationComponentPtr = CreateDefaultSubobject<UTalentAllocationComponent>(
		UTalentAllocationComponent::ComponentName);

	StateProcessorComponentPtr = CreateDefaultSubobject<UStateProcessorComponent>(
		UStateProcessorComponent::ComponentName);

	CharacterTitleComponentPtr = CreateDefaultSubobject<UCharacterTitleComponent>(
		UCharacterTitleComponent::ComponentName);
	CharacterTitleComponentPtr->SetupAttachment(RootComponent);

	ProxyProcessComponentPtr = CreateDefaultSubobject<UProxyProcessComponent>(UProxyProcessComponent::ComponentName);
	
	ConversationComponentPtr = CreateDefaultSubobject<UConversationComponent>(UConversationComponent::ComponentName);
	
	SceneActorInteractionComponentPtr = CreateDefaultSubobject<USceneActorInteractionComponent>(USceneActorInteractionComponent::ComponentName);
}

ACharacterBase::~ACharacterBase()
{
}

void ACharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	HasBeenEndedLookAt();
	
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

	auto& DelegateRef = GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		UGameplayTagsLibrary::State_Dying,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnDeathing);

	OnMoveSpeedChangedImp(CharacterAttributeSetPtr->GetMoveSpeed());

	// 这里是用于传送时或流送时重新生成这个Character，绑定之前的GroupManager数据
	if (GroupManaggerPtr)
	{
		OnRep_GroupSharedInfoChanged();
	}
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

	OriginalAIController = nullptr;

	Super::EndPlay(EndPlayReason);
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
		GroupManaggerPtr = Cast<APlanetPlayerController>(NewController)->GetGroupSharedInfo();
		OnGroupManaggerReady(GroupManaggerPtr);
	}
	else if (NewController->IsA(AHumanAIController::StaticClass()))
	{
		// GroupManaggerPtr = Cast<AHumanAIController>(NewController)->GetGroupSharedInfo();
		
		// if (auto ControllerPtr = GetController<AHumanAIController>())
		// {
		// 	ControllerPtr->SetGroupSharedInfo(GetGroupSharedInfo());
		// }
		
		// OnGroupManaggerReady(GetGroupSharedInfo());
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

class UPlanetAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentPtr;
}

AGroupManagger* ACharacterBase::GetGroupSharedInfo() const
{
	return GroupManaggerPtr;
}

UInventoryComponent* ACharacterBase::GetInventoryComponent() const
{
	return GroupManaggerPtr ? GroupManaggerPtr->GetHoldingItemsComponent() : nullptr;
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

UConversationComponent* ACharacterBase::GetConversationComponent() const
{
	return ConversationComponentPtr;
}

UCharacterTitleComponent* ACharacterBase::GetCharacterTitleComponent() const
{
	return CharacterTitleComponentPtr;
}

TSharedPtr<FCharacterProxy> ACharacterBase::GetCharacterProxy() const
{
	return GetInventoryComponent()->FindProxy_Character(GetCharacterAttributesComponent()->GetCharacterID());
}

USkeletalMeshComponent* ACharacterBase::GetCopyPoseMesh() const
{
	return CopyPoseMeshPtr;
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
	CharacterTitleComponentPtr->SetCampType(CharacterCampType);
}

bool ACharacterBase::GetIsValidTarget() const
{
	TArray<FGameplayTag> Ary{
		UGameplayTagsLibrary::State_Dying,
		UGameplayTagsLibrary::State_Buff_CantBeSlected
	};
	const auto TagContainer = FGameplayTagContainer::CreateFromArray(Ary);
	return !AbilitySystemComponentPtr->HasAnyMatchingGameplayTags(TagContainer);
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GroupManaggerPtr, COND_None);
}

void ACharacterBase::SpawnDefaultController()
{
	// Super::SpawnDefaultController();

	if ( Controller != nullptr || GetNetMode() == NM_Client)
	{
		return;
	}

	if (AIControllerClass != nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.CustomPreSpawnInitalization = [this](AActor* ActorPtr)
		{
			auto AIControllerPtr = Cast<APlanetAIController>(ActorPtr);
			if (AIControllerPtr)
			{
				AIControllerPtr->SetGroupSharedInfo(GetGroupSharedInfo());
			}
		};
		SpawnInfo.OverrideLevel = GetLevel();
		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save AI controllers into a map
		AController* NewController = GetWorld()->SpawnActor<AController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		if (NewController != nullptr)
		{
			// if successful will result in setting this->Controller 
			// as part of possession mechanics
			NewController->Possess(this);
		}
	}
	
	OriginalAIController = Controller;
}

void ACharacterBase::OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr)
{
	OnInitaliedGroupSharedInfo();

	auto NewController = GetController();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto ProxyCharacterSPtr = GetInventoryComponent()->InitialOwnerCharacterProxy(this);
	}
#endif

	ForEachComponent(false, [this](UActorComponent* ComponentPtr)
	{
		auto GroupSharedInterfacePtr = Cast<IGroupManaggerInterface>(ComponentPtr);
		if (GroupSharedInterfacePtr)
		{
			GroupSharedInterfacePtr->OnGroupManaggerReady(GroupManaggerPtr);
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
	if (!GroupManaggerPtr)
	{
		return;
	}

#if UE_EDITOR || UE_SERVER
	if (GetLocalRole() > ROLE_SimulatedProxy)
	{
		OnGroupManaggerReady(GroupManaggerPtr);
	}
#endif
	
#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (GetCharacterAttributesComponent()->bCharacterIDIsReplicated)
		{
			OnGroupManaggerReady(GroupManaggerPtr);
		}
	}
#endif
}

void ACharacterBase::OnDeathing(const FGameplayTag Tag, int32 Count)
{
}

void ACharacterBase::DoDeathing()
{
	// auto CharacterPtr = GetPawn<FPawnType>();
	// if (CharacterPtr)
	// {
	// }
}

void ACharacterBase::HasbeenInteracted(ACharacterBase* CharacterPtr)
{
}

void ACharacterBase::HasBeenLookingAt(ACharacterBase* CharacterPtr)
{
}

void ACharacterBase::HasBeenStartedLookAt(ACharacterBase* CharacterPtr)
{
}

void ACharacterBase::HasBeenEndedLookAt()
{
}

USceneActorInteractionComponent* ACharacterBase::GetSceneActorInteractionComponent() const
{
	return SceneActorInteractionComponentPtr;
}
