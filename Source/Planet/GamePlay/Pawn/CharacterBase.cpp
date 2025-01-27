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
#include "GAEvent_Helper.h"
#include "CharacterRisingTips.h"
#include "CharacterTitleComponent.h"
#include "SceneActor.h"
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

	auto& DelegateRef = GetAbilitySystemComponent()->RegisterGameplayTagEvent(
		UGameplayTagsLibrary::DeathingTag,
		EGameplayTagEventType::NewOrRemoved
	);
	OnOwnedDeathTagDelegateHandle = DelegateRef.AddUObject(this, &ThisClass::OnDeathing);

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
		GroupSharedInfoPtr = Cast<APlanetPlayerController>(NewController)->GetGroupSharedInfo();
		OnGroupSharedInfoReady(GroupSharedInfoPtr);
	}
	else if (NewController->IsA(AHumanAIController::StaticClass()))
	{
		GroupSharedInfoPtr = Cast<AHumanAIController>(NewController)->GetGroupSharedInfo();
		OnGroupSharedInfoReady(GroupSharedInfoPtr);
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

void ACharacterBase::InteractionSceneActor(ASceneActor* SceneObjPtr)
{
	InteractionSceneObj_Server(SceneObjPtr);
}

void ACharacterBase::InteractionSceneCharacter(AHumanCharacter_AI* CharacterPtr)
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

UInventoryComponent* ACharacterBase::GetInventoryComponent() const
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

void ACharacterBase::InteractionSceneObj_Server_Implementation(ASceneActor* SceneObjPtr)
{
	if (SceneObjPtr)
	{
		SceneObjPtr->HasbeenInteracted(this);
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
	CharacterTitleComponentPtr->SetCampType(CharacterCampType);
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
	// 只在 ROLE_AutonomousProxy 运行
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

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto ProxyCharacterSPtr = GetInventoryComponent()->InitialOwnerCharacterProxy(this);

		GetGroupSharedInfo()->GetTeamMatesHelperComponent()->OwnerCharacterProxyPtr = ProxyCharacterSPtr;
	}
#endif

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
