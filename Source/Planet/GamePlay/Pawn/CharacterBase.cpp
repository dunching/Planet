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
#include "SceneUnitContainer.h"

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
 
 				auto PlayerCharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
 				if (PlayerCharacterPtr)
 				{
 					SetCampType(
 						IsTeammate(PlayerCharacterPtr) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
 					);
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
	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
		InitialDefaultCharacterUnit();

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
	}

	if ((World->IsGameWorld()))
	{
		HoldingItemsComponentPtr->Proxy_Container.HoldingItemsComponentPtr = HoldingItemsComponentPtr;

		ProxyProcessComponentPtr->AllocationSkills_Container.HoldingItemsComponentPtr = HoldingItemsComponentPtr;
		ProxyProcessComponentPtr->AllocationSkills_Container.UnitProxyProcessComponentPtr = ProxyProcessComponentPtr;

		CDCaculatorComponentPtr->CD_FASI_Container.CDCaculatorComponentPtr = CDCaculatorComponentPtr;

		StateProcessorComponentPtr->CharacterStateInfo_FASI_Container.StateProcessorComponent = StateProcessorComponentPtr;

		TalentAllocationComponentPtr->Talent_FASI_Container.TalentAllocationComponentPtr = TalentAllocationComponentPtr;
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

UBaseFeatureComponent* ACharacterBase::GetBaseFeatureComponent()const
{
	return BaseFeatureComponentPtr;
}

UStateProcessorComponent* ACharacterBase::GetStateProcessorComponent() const
{
	return StateProcessorComponentPtr;
}

UProxyProcessComponent* ACharacterBase::GetProxyProcessComponent()const
{
	return ProxyProcessComponentPtr;
}

UGroupMnaggerComponent* ACharacterBase::GetGroupMnaggerComponent()const
{
	return GroupMnaggerComponentPtr;
}

UCDCaculatorComponent* ACharacterBase::GetCDCaculatorComponent() const
{
	return CDCaculatorComponentPtr;
}

TSharedPtr<FCharacterProxy> ACharacterBase::GetCharacterUnit()const
{
	return HoldingItemsComponentPtr->CharacterProxySPtr;
}

void ACharacterBase::InitialDefaultCharacterUnit()
{
	TSharedPtr<FCharacterProxy>CharacterUnitPtr = nullptr;

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		CharacterUnitPtr = HoldingItemsComponentPtr->InitialDefaultCharacter();
	}
#endif

	auto CharacterAttributes = GetCharacterAttributesComponent()->GetCharacterAttributes();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		CharacterUnitPtr = HoldingItemsComponentPtr->InitialDefaultCharacter();
	}
#endif
	
	// 

	// 
	if (CharacterUnitPtr)
	{
		auto TableRowUnit_CharacterInfoPtr = CharacterUnitPtr->GetTableRowUnit_CharacterInfo();
	}
	else
	{
		CharacterUnitPtr = HoldingItemsComponentPtr->InitialDefaultCharacter();
		checkNoEntry();
	}
//	GetCharacterAttributesComponent()->CharacterAttributes = TableRowUnit_CharacterInfoPtr->CharacterAttributes;
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

void ACharacterBase::SetCampType_Implementation(ECharacterCampType CharacterCampType)
{
 	if (CharacterTitlePtr)
 	{
 		CharacterTitlePtr->SetCampType(CharacterCampType);
 	}
}

bool ACharacterBase::GetIsValidTarget() const
{
	TArray<FGameplayTag>Ary{
		UGameplayTagsSubSystem::GetInstance()->DeathingTag,
			UGameplayTagsSubSystem::GetInstance()->State_Buff_CantBeSlected
	};
	const auto TagContainer = FGameplayTagContainer::CreateFromArray(Ary);
	return !AbilitySystemComponentPtr->HasAnyMatchingGameplayTags(TagContainer);
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
