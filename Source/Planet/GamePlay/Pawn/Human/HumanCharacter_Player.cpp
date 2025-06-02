#include "HumanCharacter_Player.h"

#include "AssetRefMap.h"
#include "ChallengeEntry.h"
#include "CollisionDataStruct.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/GameplayCameraComponent.h"

#include "GravitySpringArmComponent.h"
#include "PlayerComponent.h"
#include "GroupManagger.h"
#include "GuideSubSystem.h"
#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "MainHUD.h"
#include "PlanetPlayerController.h"

#include "ChallengeEntry.h"
#include "CharacterAbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagsLibrary.h"
#include "InteractionList.h"
#include "MainHUDLayout.h"
#include "SceneActor.h"
#include "HumanInteractionWithNPC.h"
#include "HumanInteractionWithChallengeEntry.h"
#include "InputActions.h"
#include "PlanetGameViewportClient.h"
#include "ResourceBoxBase.h"
#include "STT_CommonData.h"
#include "UIManagerSubSystem.h"
#include "PlanetGameplayCameraComponent.h"
#include "TeamMatesHelperComponent.h"

namespace HumanProcessor
{
	class FHumanInteractionWithNPCProcessor;
	class FHumanInteractionWithChallengeEntryProcessor;
}

TArray<TWeakObjectPtr<ACharacterBase>> UCharacterPlayerStateProcessorComponent::GetTargetCharactersAry() const
{
	TArray<TWeakObjectPtr<ACharacterBase>> Result;
	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		if (auto GroupManaggerPtr = CharacterPtr->GetGroupManagger())
		{
			auto ForceKnowCharater = GroupManaggerPtr->GetTeamMatesHelperComponent()->GetForceKnowCharater();
			if (ForceKnowCharater.IsValid())
			{
				Result.Add(ForceKnowCharater.Get());
			}
		}
	}
	return Result;
}

void UCharacterPlayerStateProcessorComponent::FocusTarget()
{
	auto OnwerActorPtr = GetOwner<FOwnerPawnType>();
	if (!OnwerActorPtr)
	{
		return;
	}

	auto PCPtr = OnwerActorPtr->GetController<APlanetPlayerController>();
	if (!PCPtr)
	{
		return;
	}

	auto PlayerCameraManagerPtr = UGameplayStatics::GetPlayerCameraManager(GetWorldImp(), 0);
	if (PlayerCameraManagerPtr)
	{
		FVector OutCamLoc;
		FRotator OutCamRot;
		PlayerCameraManagerPtr->GetCameraViewPoint(OutCamLoc, OutCamRot);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OnwerActorPtr);

		FHitResult OutHit;
		if (GetWorldImp()->LineTraceSingleByObjectType(
		                                               OutHit,
		                                               OutCamLoc,
		                                               OutCamLoc + (OutCamRot.Vector() * 1000),
		                                               ObjectQueryParams,
		                                               Params
		                                              ))
		{
			auto FocusCharactersAry_ = GetTargetCharactersAry();
			if (FocusCharactersAry_.IsValidIndex(0) && (FocusCharactersAry_[0] == OutHit.GetActor()))
			{
			}
			else
			{
				auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
				if (TargetCharacterPtr)
				{
					SetFocusCharactersAry(TargetCharacterPtr);
					return;
				}
			}
		}
	}

	ClearFocusCharactersAry();
}

inline void UCharacterPlayerStateProcessorComponent::OnFocusCharacterDestroyed(
	AActor* DestroyedActor
	)
{
	ClearFocusCharactersAry();
}

void UCharacterPlayerStateProcessorComponent::OnGameplayEffectTagCountChanged(
	const FGameplayTag Tag,
	int32 Count
	)
{
	Super::OnGameplayEffectTagCountChanged(Tag, Count);

	auto Lambda = [&]
	{
		const auto Value = Count > 0;
		return Value;
	};
	if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_Dying))
	{
		ClearFocusCharactersAry();
	}
	else if (Tag.MatchesTagExact(UGameplayTagsLibrary::State_Buff_CantBeSlected))
	{
		ClearFocusCharactersAry();
	}
}

void UCharacterPlayerStateProcessorComponent::SetFocusCharactersAry(
	ACharacterBase* TargetCharacterPtr
	)
{
	if (TargetCharacterPtr && TargetCharacterPtr != PreviousFocusCharactersPtr)
	{
		PreviousFocusCharactersPtr = TargetCharacterPtr;

		TargetCharacterPtr->OnDestroyed.AddDynamic(this, &ThisClass::OnFocusCharacterDestroyed);
		OnGameplayEffectTagCountChangedHandle = TargetCharacterPtr->GetCharacterAbilitySystemComponent()->
		                                                            RegisterGenericGameplayTagEvent().AddUObject(
			                                                             this,
			                                                             &ThisClass::OnGameplayEffectTagCountChanged
			                                                            );
	}
	else
	{
		return;
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		SetFocusCharactersAry_Server(TargetCharacterPtr);
	}
#endif

	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		CharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->SetFocusCharactersAry(TargetCharacterPtr);
	}
}

void UCharacterPlayerStateProcessorComponent::ClearFocusCharactersAry()
{
	if (PreviousFocusCharactersPtr)
	{
		PreviousFocusCharactersPtr->OnDestroyed.RemoveDynamic(this, &ThisClass::OnFocusCharacterDestroyed);
		PreviousFocusCharactersPtr->GetCharacterAbilitySystemComponent()->
		                            RegisterGenericGameplayTagEvent().Remove(OnGameplayEffectTagCountChangedHandle);
		PreviousFocusCharactersPtr = nullptr;
	}

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		ClearFocusCharactersAry_Server();
	}
#endif

	auto CharacterPtr = GetOwner<FOwnerPawnType>();
	if (CharacterPtr)
	{
		CharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->ClearFocusCharactersAry();
	}
}

void UCharacterPlayerStateProcessorComponent::ClearFocusCharactersAry_Server_Implementation()
{
	ClearFocusCharactersAry();
}

void UCharacterPlayerStateProcessorComponent::SetFocusCharactersAry_Server_Implementation(
	ACharacterBase* TargetCharacterPtr
	)
{
	SetFocusCharactersAry(TargetCharacterPtr);
}

void UPlayerConversationComponent::DisplaySentence_Player(
	const FTaskNode_Conversation_SentenceInfo& Sentence,
	const std::function<void()>& SentenceStop_
	)
{
	SentenceStop = SentenceStop_;

	OnPlayerHaveNewSentence.Broadcast(true, Sentence);
}

void UPlayerConversationComponent::CloseConversationborder_Player()
{
	OnPlayerHaveNewSentence.Broadcast(false, FTaskNode_Conversation_SentenceInfo());
}

AHumanCharacter_Player::AHumanCharacter_Player(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(
	        ObjectInitializer.
	        SetDefaultSubobjectClass<UPlayerConversationComponent>(
	                                                               UPlayerConversationComponent::ComponentName
	                                                              ).
	        SetDefaultSubobjectClass<USceneCharacterPlayerInteractionComponent>(
	                                                                            USceneCharacterPlayerInteractionComponent::ComponentName
	                                                                           ).
	        SetDefaultSubobjectClass<UCharacterPlayerStateProcessorComponent>(
	                                                                          UCharacterPlayerStateProcessorComponent::ComponentName
	                                                                         )
	       )
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	GameplayCameraComponentPtr = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCameraComponent"));
	GameplayCameraComponentPtr->SetupAttachment(RootComponent);

	PlayerComponentPtr = CreateDefaultSubobject<UPlayerComponent>(UPlayerComponent::ComponentName);
}

void AHumanCharacter_Player::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// BUG,先暂时这样处理
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
		                                TimerHandle,
		                                [this]
		                                {
			                                AdjustCamera(UGameOptions::GetInstance()->DefaultBoomLength);
		                                },
		                                1,
		                                true
		                               );
	}
#endif
}

void AHumanCharacter_Player::PossessedBy(
	AController* NewController
	)
{
	Super::PossessedBy(NewController);

	PlayerComponentPtr->PossessedBy(Cast<APlayerController>(NewController));

	auto GroupsHelperSPtr = GetGroupManagger()->GetTeamMatesHelperComponent();
	if (GroupsHelperSPtr)
	{
	}
}

void AHumanCharacter_Player::OnRep_Controller()
{
	Super::OnRep_Controller();

	PlayerComponentPtr->PossessedBy(Cast<APlayerController>(Controller));
}

void AHumanCharacter_Player::UnPossessed()
{
	Super::UnPossessed();
}

bool AHumanCharacter_Player::TeleportTo(
	const FVector& DestLocation,
	const FRotator& DestRotation,
	bool bIsATest,
	bool bNoCheck
	)
{
	PlayerComponentPtr->TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);

	return Super::TeleportTo(DestLocation, DestRotation, bIsATest, bNoCheck);
}

void AHumanCharacter_Player::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent
	)
{
	// 只在 ROLE_AutonomousProxy 运行
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerComponentPtr->SetupPlayerInputComponent(PlayerInputComponent);
}

void AHumanCharacter_Player::OnRep_GroupSharedInfoChanged()
{
	Super::OnRep_GroupSharedInfoChanged();

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
	}
#endif

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// 在SetPawn之后调用
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>(
			 [this](
			 auto NewProcessor
			 )
			 {
				 NewProcessor->SetPawn(Cast<ThisClass>(this));
			 }
			);

		// 
		UGuideSubSystem::GetInstance()->InitializeMainThread();
		UGuideSubSystem::GetInstance()->ActiveMainThread();
	}
#endif
}

void AHumanCharacter_Player::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
	Super::OnGroupManaggerReady(NewGroupSharedInfoPtr);

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// 显示
		Cast<AMainHUD>(GetController<APlanetPlayerController>()->MyHUD)->InitalHUD();
	}
#endif
}

UPlayerConversationComponent* AHumanCharacter_Player::GetPlayerConversationComponent() const
{
	return Cast<UPlayerConversationComponent>(ConversationComponentPtr);
}

USceneCharacterPlayerInteractionComponent* AHumanCharacter_Player::GetSceneCharacterPlayerInteractionComponent() const
{
	return Cast<USceneCharacterPlayerInteractionComponent>(SceneActorInteractionComponentPtr);
}

UCharacterPlayerStateProcessorComponent* AHumanCharacter_Player::GetCharacterPlayerStateProcessorComponent() const
{
	return Cast<UCharacterPlayerStateProcessorComponent>(StateProcessorComponentPtr);
}

TObjectPtr<UGameplayCameraComponent> AHumanCharacter_Player::GetGameplayCameraComponent() const
{
	return GameplayCameraComponentPtr;
}

TObjectPtr<UPlayerComponent> AHumanCharacter_Player::GetPlayerComponent() const
{
	return PlayerComponentPtr;
}

void AHumanCharacter_Player::UpdateSightActor()
{
	FVector OutCamLoc = FVector::ZeroVector;
	FRotator OutCamRot = FRotator::ZeroRotator;
	auto CameraManagerPtr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManagerPtr)
	{
		CameraManagerPtr->GetCameraViewPoint(OutCamLoc, OutCamRot);
	}

	auto StartPt = OutCamLoc;
	auto StopPt = OutCamLoc + (OutCamRot.Vector() * 1000);

	FHitResult Result;

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);

	ISceneActorInteractionInterface* TempLookAtSceneObjPtr = nullptr;
	if (GetWorld()->LineTraceSingleByChannel(
	                                         Result,
	                                         StartPt,
	                                         StopPt,
	                                         SceneActor_Channel,
	                                         Params
	                                        )
	)
	{
		if (Cast<ISceneActorInteractionInterface>(Result.GetActor()))
		{
			TempLookAtSceneObjPtr = Cast<ISceneActorInteractionInterface>(Result.GetActor());
		}
	}

	if (TempLookAtSceneObjPtr)
	{
		if (LookAtSceneActorPtr)
		{
			if (TempLookAtSceneObjPtr == LookAtSceneActorPtr)
			{
			}
			else
			{
				StartLookAt(TempLookAtSceneObjPtr);
			}
		}
		else
		{
			StartLookAt(TempLookAtSceneObjPtr);
		}
		LookingAt(TempLookAtSceneObjPtr);

		TempLookAtSceneObjPtr->HasBeenLookingAt(this);
	}
	else
	{
		EndLookAt();
		LookAtSceneActorPtr = nullptr;
	}
}

void AHumanCharacter_Player::StartLookAt(
	ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr
	)
{
	if (SceneActorInteractionInterfacePtr)
	{
		if (LookAtSceneActorPtr)
		{
			if (SceneActorInteractionInterfacePtr == LookAtSceneActorPtr)
			{
			}
			else
			{
				SceneActorInteractionInterfacePtr->HasBeenStartedLookAt(this);
				LookAtSceneActorPtr->HasBeenEndedLookAt();
				LookAtSceneActorPtr = SceneActorInteractionInterfacePtr;
			}
		}
		else
		{
			SceneActorInteractionInterfacePtr->HasBeenStartedLookAt(this);
			LookAtSceneActorPtr = SceneActorInteractionInterfacePtr;
		}
	}
	else
	{
		if (LookAtSceneActorPtr)
		{
			LookAtSceneActorPtr->HasBeenEndedLookAt();
		}

		LookAtSceneActorPtr = nullptr;
	}
}

void AHumanCharacter_Player::LookingAt(
	ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr
	)
{
	if (SceneActorInteractionInterfacePtr)
	{
		SceneActorInteractionInterfacePtr->HasBeenLookingAt(this);
	}
}

void AHumanCharacter_Player::EndLookAt()
{
	if (LookAtSceneActorPtr)
	{
		LookAtSceneActorPtr->HasBeenEndedLookAt();
	}

	if (InteractionListPtr)
	{
		InteractionListPtr->RemoveFromParent();
		InteractionListPtr = nullptr;
	}
}

bool AHumanCharacter_Player::InteractionSceneActor(
	ASceneActor* SceneObjPtr,
	ISceneActorInteractionInterface* SceneActorInteractionInterfacePtr
	)
{
	if (!SceneObjPtr)
	{
		return false;
	}

	if (!SceneActorInteractionInterfacePtr)
	{
		return false;
	}

	if (SceneObjPtr->IsA(AResourceBoxBase::StaticClass()))
	{
		//
		GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Broadcast(SceneActorInteractionInterfacePtr);

		InteractionSceneObj_Server(SceneObjPtr);
	}
	else if (SceneObjPtr->IsA(AChallengeEntry::StaticClass()))
	{
		//
		GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Broadcast(SceneActorInteractionInterfacePtr);

		// 隐藏交互提示
		if (SceneObjPtr)
		{
			SceneActorInteractionInterfacePtr->HasBeenEndedLookAt();
		}

		// 
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<
			HumanProcessor::FHumanInteractionWithChallengeEntryProcessor>(
			                                                              [SceneObjPtr](
			                                                              auto NewProcessor
			                                                              )
			                                                              {
				                                                              NewProcessor->TargetPtr = Cast<
					                                                              AChallengeEntry>(SceneObjPtr);
			                                                              }
			                                                             );
	}
	else
	{
		//
		GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Broadcast(SceneActorInteractionInterfacePtr);

		InteractionSceneObj_Server(SceneObjPtr);
	}

	return true;
}

bool AHumanCharacter_Player::InteractionSceneCharacter(
	AHumanCharacter_AI* CharacterPtr
	)
{
	if (!CharacterPtr)
	{
		return false;
	}

	if (!CharacterPtr->GetSceneActorInteractionComponent()->GetIsEnableInteraction())
	{
		return false;
	}

	//
	GetSceneCharacterPlayerInteractionComponent()->OnPlayerInteraction.Broadcast(CharacterPtr);

	// 隐藏交互提示
	if (CharacterPtr)
	{
		CharacterPtr->HasBeenEndedLookAt();
	}

	// 
	UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanInteractionWithNPCProcessor>(
		 [CharacterPtr](
		 auto NewProcessor
		 )
		 {
			 NewProcessor->CharacterPtr = CharacterPtr;
		 }
		);

	return true;
}

void AHumanCharacter_Player::InteractionSceneObj_Server_Implementation(
	ASceneActor* SceneObjPtr
	)
{
	if (!SceneObjPtr)
	{
		return;
	}
	
	auto InterfacePtr = Cast<ISceneActorInteractionInterface>(SceneObjPtr);
	if (!InterfacePtr)
	{
		return;
	}
	
	InterfacePtr->HasbeenInteracted(this);
}
