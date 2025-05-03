#include "PlanetPlayerController.h"

#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include <Engine/Engine.h>
#include <IXRTrackingSystem.h>
#include <IXRCamera.h>

#include "AssetRefMap.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/HUD.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetStringLibrary.h"

#include "InputProcessorSubSystem.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"
#include "InputActions.h"
#include "UIManagerSubSystem.h"
#include "CharacterBase.h"
#include "TeamMatesHelperComponent.h"
#include "HumanAIController.h"
#include "ItemProxy_Minimal.h"
#include "PlanetControllerInterface.h"
#include "NavgationSubSysetem.h"
#include "FocusIcon.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter_Player.h"
#include "KismetGravityLibrary.h"
#include "CollisionDataStruct.h"
#include "EventSubjectComponent.h"
#include "GameMode_Main.h"
#include "GeneratorBase.h"
#include "GeneratorColony_ByInvoke.h"
#include "LogWriter.h"
#include "GroupManagger.h"
#include "InventoryComponent.h"
#include "MainHUD.h"
#include "PlanetWorldSettings.h"
#include "GuideActor.h"
#include "GroupManagger_Player.h"
#include "HumanCharacter_AI.h"
#include "OpenWorldSystem.h"
#include "PlayerGameplayTasks.h"

static TAutoConsoleVariable<int32> PlanetPlayerController_DrawControllerRotation(
	 TEXT("PlanetPlayerController.DrawControllerRotation"),
	 0,
	 TEXT("")
	 TEXT(" default: 0")
	);

APlanetPlayerController::APlanetPlayerController(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	EventSubjectComponentPtr = CreateDefaultSubobject<UEventSubjectComponent>(
	                                                                          UEventSubjectComponent::ComponentName
	                                                                         );

	GameplayTasksComponentPtr = CreateDefaultSubobject<UPlayerControllerGameplayTasksComponent>(
		 UPlayerControllerGameplayTasksComponent::ComponentName
		);
}

void APlanetPlayerController::ServerSpawnGeneratorActor_Implementation(
	const TSoftObjectPtr<AGeneratorColony_ByInvoke>& GeneratorBasePtr
	)
{
	GeneratorBasePtr->SpawnGeneratorActor();
}

void APlanetPlayerController::ServerDestroyActor_Implementation(
	AActor* ActorPtr
	)
{
	if (ActorPtr)
	{
		ActorPtr->Destroy();
	}
}

void APlanetPlayerController::ServerSpawnCharacter_Implementation(
	TSubclassOf<AHumanCharacter_AI> CharacterClass,
	const FGuid& ID,
	const FTransform& Transform
	)
{
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.CustomPreSpawnInitalization = [this, ID](
		auto ActorPtr
		)
		{
			auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
			if (AICharacterPtr)
			{
				AICharacterPtr->GetCharacterAttributesComponent()->SetCharacterID(ID);
			}
		};

	auto Result =
		GetWorld()->SpawnActor<AHumanCharacter_AI>(
		                                           CharacterClass,
		                                           Transform,
		                                           SpawnParameters
		                                          );
	if (Result)
	{
	}
}

void APlanetPlayerController::IncreaseCD_Implementation(
	int32 CD
	)
{
	auto CharacterPtr = Cast<ACharacterBase>(GetPawn());
	if (CharacterPtr)
	{
		auto InTags = FGameplayTagContainer::EmptyContainer;

		InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

		const auto GameplayEffectHandleAry = CharacterPtr->GetCharacterAbilitySystemComponent()->
		                                                   GetActiveEffectsWithAllTags(
			                                                    InTags
			                                                   );
		if (!GameplayEffectHandleAry.IsEmpty())
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->
			              ModifyActiveEffectStartTime(
			                                          GameplayEffectHandleAry[0],
			                                          CD
			                                         );
		}
	}
}

void APlanetPlayerController::BuyProxys_Implementation(
	ACharacterBase* InTraderCharacterPtr,
	const FGameplayTag& BuyProxyTag,
	const FGuid& BuyProxyID,
	int32 Num,
	const FGameplayTag& CostProxyType,
	int32 Cost
	)
{
	auto InventoryComponentPtr = GetGroupManagger()->GetInventoryComponent();
	if (!InventoryComponentPtr)
	{
		return;
	}

	auto CoinProxySPtr = InventoryComponentPtr->FindProxy_Coin(CostProxyType);
	if (!(CoinProxySPtr && Cost <= CoinProxySPtr->GetNum()))
	{
		return;
	}

	if (!InTraderCharacterPtr)
	{
		return;
	}

	auto TraderInventoryComponentPtr = InTraderCharacterPtr->GetGroupManagger()->GetInventoryComponent();
	auto TargetProxySPtr = TraderInventoryComponentPtr->FindProxy(BuyProxyID);
	if (!TargetProxySPtr)
	{
		return;
	}

	const auto RemainNum = GetProxyNum(TargetProxySPtr);
	if (Num > RemainNum)
	{
		return;
	}

	InventoryComponentPtr->AddProxyNum(BuyProxyTag, Num);

	InventoryComponentPtr->RemoveProxyNum(CoinProxySPtr->GetID(), Cost);

	TraderInventoryComponentPtr->RemoveProxyNum(BuyProxyID, Num);
}

void APlanetPlayerController::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GroupManaggerPtr, COND_None);
	// DOREPLIFETIME_CONDITION(ThisClass, MainLineGuidePtr, COND_AutonomousOnly);
}

void APlanetPlayerController::BeginPlay()
{
	Super::BeginPlay();

#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
		// 因为Pawn是通过网络同步过来的，所以不在OnPoss里面去做
		auto CurrentPawn = GetPawn();
		if (CurrentPawn->IsA(AHumanCharacter::StaticClass()))
		{
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
		}
	}
#endif
}

void APlanetPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitialGroupSharedInfo();
}

void APlanetPlayerController::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

void APlanetPlayerController::PlayerTick(
	float DeltaTime
	)
{
	Super::PlayerTick(DeltaTime);
}

void APlanetPlayerController::OnPossess(
	APawn* InPawn
	)
{
	bool bIsNewPawn = (InPawn != GetPawn());

	if (bIsNewPawn)
	{
	}

	Super::OnPossess(InPawn);

	if (bIsNewPawn)
	{
	}

	if (InPawn)
	{
		// 注意：PC并非是在此处绑定，这段仅为测试
		BindPCWithCharacter();

		if (InPawn->IsA(AHumanCharacter::StaticClass()))
		{
#if UE_EDITOR || UE_SERVER
			if (GetNetMode() == NM_DedicatedServer)
			{
				if (InPawn->IsA(AHumanCharacter::StaticClass()))
				{
					if (GroupManaggerPtr)
					{
						GroupManaggerPtr->SetOwnerCharacterProxyPtr(Cast<AHumanCharacter>(InPawn));
					}
				}
			}
#endif
		}
		else if (InPawn->IsA(AHorseCharacter::StaticClass()))
		{
			auto PreviousPawnPtr = UInputProcessorSubSystem::GetInstance()->GetCurrentAction()->GetOwnerActor();

			UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HorseProcessor::FHorseRegularProcessor>(
				 [this, InPawn](
				 auto NewProcessor
				 )
				 {
					 NewProcessor->SetPawn(Cast<AHorseCharacter>(InPawn));
				 }
				);
		}
	}
}

void APlanetPlayerController::OnUnPossess()
{
	APawn* CurrentPawn = GetPawn();

	auto CharacterPtr = Cast<FPawnType>(CurrentPawn);
	if (CharacterPtr)
	{
	}

	Super::OnUnPossess();
}

void APlanetPlayerController::SetPawn(
	APawn* InPawn
	)
{
	Super::SetPawn(InPawn);
}

bool APlanetPlayerController::InputKey(
	const FInputKeyParams& Params
	)
{
	auto Result = Super::InputKey(Params);

	return Result;
}

void APlanetPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void APlanetPlayerController::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		NewGroupSharedInfoPtr->GetTeamMatesHelperComponent()->SwitchTeammateOption(ETeammateOption::kFollow);
	}
#endif

	ForEachComponent(
	                 false,
	                 [this](
	                 UActorComponent* ComponentPtr
	                 )
	                 {
		                 auto GroupSharedInterfacePtr = Cast<IGroupManaggerInterface>(ComponentPtr);
		                 if (GroupSharedInterfacePtr)
		                 {
			                 GroupSharedInterfacePtr->OnGroupManaggerReady(GroupManaggerPtr);
		                 }
	                 }
	                );
}

void APlanetPlayerController::ResetGroupmateProxy(
	FCharacterProxy* NewGourpMateProxyPtr
	)
{
}

UPlanetAbilitySystemComponent* APlanetPlayerController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAbilitySystemComponent();
}

AGroupManagger* APlanetPlayerController::GetGroupManagger() const
{
	return GroupManaggerPtr;
}

void APlanetPlayerController::SetGroupSharedInfo(
	AGroupManagger* InGroupSharedInfoPtr
	)
{
}

UInventoryComponent* APlanetPlayerController::GetInventoryComponent() const
{
	return GroupManaggerPtr ? GroupManaggerPtr->GetInventoryComponent() : nullptr;
}

UCharacterAttributesComponent* APlanetPlayerController::GetCharacterAttributesComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAttributesComponent();
}

UTalentAllocationComponent* APlanetPlayerController::GetTalentAllocationComponent() const
{
	return GetPawn<FPawnType>()->GetTalentAllocationComponent();
}

TWeakObjectPtr<ACharacterBase> APlanetPlayerController::GetTeamFocusTarget() const
{
	if (GetGroupManagger() && GetGroupManagger()->GetTeamMatesHelperComponent())
	{
		return GetGroupManagger()->GetTeamMatesHelperComponent()->GetForceKnowCharater();
	}

	return nullptr;
}

TSharedPtr<FCharacterProxy> APlanetPlayerController::GetCharacterProxy()
{
	return GetPawn<FPawnType>()->GetCharacterProxy();
}

ACharacterBase* APlanetPlayerController::GetRealCharacter() const
{
	return Cast<ACharacterBase>(GetPawn());
}

void APlanetPlayerController::OnHPChanged(
	int32 CurrentValue
	)
{
	if (CurrentValue <= 0)
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(
		                                                       FGameplayTagContainer{
			                                                       UGameplayTagsLibrary::BaseFeature_Dying
		                                                       }
		                                                      );
		GetAbilitySystemComponent()->OnAbilityEnded.AddLambda(
		                                                      [this](
		                                                      const FAbilityEndedData& AbilityEndedData
		                                                      )
		                                                      {
			                                                      // for (auto Iter : AbilityEndedData.AbilityThatEnded->AbilityTags)
			                                                      // {
			                                                      // 	if (Iter == UGameplayTagsLibrary::State_Dying)
			                                                      // 	{
			                                                      // 		// TODO 
			                                                      // 		//					Destroy();
			                                                      // 	}
			                                                      // }
		                                                      }
		                                                     );
	}
}

UEventSubjectComponent* APlanetPlayerController::GetEventSubjectComponent() const
{
	return EventSubjectComponentPtr;
}

TObjectPtr<UPlayerControllerGameplayTasksComponent> APlanetPlayerController::GetGameplayTasksComponent() const
{
	return GameplayTasksComponentPtr;
}

void APlanetPlayerController::EntryChallengeLevel_Implementation(
	ETeleport Teleport
	)
{
	UOpenWorldSubSystem::GetInstance()->SwitchDataLayer(Teleport, this);
}

void APlanetPlayerController::BindPCWithCharacter()
{
}

TSharedPtr<FCharacterProxy> APlanetPlayerController::InitialCharacterProxy(
	ACharacterBase* CharaterPtr
	)
{
	return CharaterPtr->GetCharacterProxy();
}

void APlanetPlayerController::InitialGroupSharedInfo()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.Owner = this;
		SpawnParameters.CustomPreSpawnInitalization = [](
			AActor* ActorPtr
			)
			{
				PRINTINVOKEINFO();
				auto GroupManaggerPtr = Cast<AGroupManagger>(ActorPtr);
				if (GroupManaggerPtr)
				{
					GroupManaggerPtr->GroupID = FGuid::NewGuid();
				}
			};

		GroupManaggerPtr = GetWorld()->SpawnActor<AGroupManagger_Player>(
		                                                                 AGroupManagger_Player::StaticClass(),
		                                                                 SpawnParameters
		                                                                );

		OnGroupManaggerReady(GroupManaggerPtr);
	}
#endif
}

void APlanetPlayerController::MakeTrueDamege_Implementation(
	const TArray<FString>& Args
	)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(0))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(
	                                            OutHit,
	                                            OutCamLoc,
	                                            OutCamLoc + (OutCamRot.Vector() * 1000),
	                                            ObjectQueryParams,
	                                            Params
	                                           ))
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
		if (TargetCharacterPtr)
		{
			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			auto ASCPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();
			FGameplayEffectSpecHandle SpecHandle =
				ICPtr->MakeOutgoingSpec(
				                        UAssetRefMap::GetInstance()->DamageClass,
				                        1,
				                        ICPtr->MakeEffectContext()
				                       );

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::Proxy_Weapon_Test);

			int32 Damege = 0;
			LexFromString(Damege, *Args[0]);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base,
			                                               Damege
			                                              );

			CharacterPtr->GetCharacterAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
				 *SpecHandle.Data.Get(),
				 ASCPtr,
				 ASCPtr->GetPredictionKeyForNewAction()
				);
		}
	}
}

void APlanetPlayerController::MakeTrueDamegeInArea_Implementation(
	const TArray<FString>& Args
	)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(1))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	TArray<FOverlapResult> OutHit;
	if (GetWorld()->OverlapMultiByObjectType(
	                                         OutHit,
	                                         OutCamLoc,
	                                         FQuat::Identity,
	                                         ObjectQueryParams,
	                                         FCollisionShape::MakeSphere(
	                                                                     UKismetStringLibrary::Conv_StringToInt(Args[1])
	                                                                    ),
	                                         Params
	                                        ))
	{
		for (const auto& Iter : OutHit)
		{
			auto TargetCharacterPtr = Cast<AHumanCharacter>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
				auto ASCPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();
				FGameplayEffectSpecHandle SpecHandle =
					ICPtr->MakeOutgoingSpec(
					                        UAssetRefMap::GetInstance()->DamageClass,
					                        1,
					                        ICPtr->MakeEffectContext()
					                       );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::Proxy_Weapon_Test);

				int32 Damege = 0;
				LexFromString(Damege, *Args[0]);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Base,
				                                               Damege
				                                              );

				CharacterPtr->GetCharacterAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
					 *SpecHandle.Data.Get(),
					 ASCPtr,
					 ASCPtr->GetPredictionKeyForNewAction()
					);
			}
		}
	}
}

void APlanetPlayerController::MakeTherapy_Implementation(
	const TArray<FString>& Args
	)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(0))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(
	                                            OutHit,
	                                            OutCamLoc,
	                                            OutCamLoc + (OutCamRot.Vector() * 1000),
	                                            ObjectQueryParams,
	                                            Params
	                                           ))
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
		if (TargetCharacterPtr)
		{
		}
	}
}

void APlanetPlayerController::MakeRespawn_Implementation(
	const TArray<FString>& Args
	)
{
	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr && !Args.IsValidIndex(0))
	{
		return;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(CharacterPtr);

	FVector OutCamLoc = CharacterPtr->GetActorLocation();
	FRotator OutCamRot = CharacterPtr->GetActorRotation();

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(
	                                            OutHit,
	                                            OutCamLoc,
	                                            OutCamLoc + (OutCamRot.Vector() * 1000),
	                                            ObjectQueryParams,
	                                            Params
	                                           ))
	{
		auto TargetCharacterPtr = Cast<AHumanCharacter>(OutHit.GetActor());
		if (TargetCharacterPtr)
		{
		}
	}
}

void APlanetPlayerController::OnRep_GroupSharedInfoChanged()
{
	if (!GroupManaggerPtr)
	{
		return;
	}

	OnGroupManaggerReady(GroupManaggerPtr);

	// auto PlayerCharacterPtr = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	// if (PlayerCharacterPtr)
	// {
	// 	SetCampType(
	// 		IsTeammate(PlayerCharacterPtr) ? ECharacterCampType::kTeamMate : ECharacterCampType::kEnemy
	// 	);
	// }
}

void APlanetPlayerController::OnRep_WolrdProcess()
{
}

inline void APlanetPlayerController::AddProxy_Implementation(
	const FGameplayTag& ProxyType,
	int32 Num
	)
{
	auto InventoryComponentPtr = GetGroupManagger()->GetInventoryComponent();
	if (InventoryComponentPtr)
	{
		InventoryComponentPtr->AddProxyNum(
		                                   ProxyType,
		                                   Num
		                                  );
	}
}
