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

#include "InputProcessorSubSystemBase.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanRegularProcessor.h"
#include "HorseRegularProcessor.h"
#include "InputActions.h"
#include "UIManagerSubSystem.h"
#include "CharacterBase.h"
#include "TeamMatesHelperComponentBase.h"
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
#include "PlanetGenerator.h"
#include "GeneratorColony_ByInvoke.h"
#include "LogWriter.h"
#include "GroupManagger.h"
#include "GroupManagger_NPC.h"
#include "InventoryComponent.h"
#include "MainHUD.h"
#include "PlanetWorldSettings.h"
#include "GuideActorBase.h"
#include "GroupManagger_Player.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Coin.h"
#include "ModifyItemProxyStrategy.h"
#include "OpenWorldSystem.h"
#include "PlanetGameViewportClient.h"
#include "PlayerGameplayTasks.h"
#include "DataTableCollection.h"
#include "InputProcessorSubSystem_Imp.h"
#include "PlanetPlayerState.h"
#include "RewardsTD.h"
#include "TeamMatesHelperComponent.h"

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

	bAutoManageActiveCameraTarget = false;
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

void APlanetPlayerController::ServerSpawnCharacterAry_Implementation(
	const TArray<TSubclassOf<AHumanCharacter_AI>>& CharacterClassAry,
	const TArray<FGuid>& IDAry,
	const TArray<FTransform>& TransformAry,
	ETeammateOption TeammateOption
	)
{
	FActorSpawnParameters GroupManaggerSpawnParameters;

	GroupManaggerSpawnParameters.Owner = this;
	GroupManaggerSpawnParameters.CustomPreSpawnInitalization = [](
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

	auto GroupManagger_NPCPtr = GetWorld()->SpawnActor<AGroupManagger_NPC>(
	                                                                       AGroupManagger_NPC::StaticClass(),
	                                                                       GroupManaggerSpawnParameters
	                                                                      );
	GroupManagger_NPCPtr->GetTeamMatesHelperComponent()->SwitchTeammateOption(
	                                                                          TeammateOption
	                                                                         );

	if ((CharacterClassAry.Num() == IDAry.Num()) && (IDAry.Num() == TransformAry.Num()))
	{
		for (int32 Index = 0; Index < CharacterClassAry.Num(); Index++)
		{
			FActorSpawnParameters SpawnParameters;

			SpawnParameters.CustomPreSpawnInitalization = [this, &IDAry, Index, GroupManagger_NPCPtr](
				auto ActorPtr
				)
				{
					auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
					if (AICharacterPtr)
					{
						AICharacterPtr->SetGroupSharedInfo(GroupManagger_NPCPtr);
						AICharacterPtr->GetCharacterAttributesComponent()->SetCharacterID(IDAry[Index]);
					}
				};

			auto AICharacterPtr =
				GetWorld()->SpawnActor<AHumanCharacter_AI>(
				                                           CharacterClassAry[Index],
				                                           TransformAry[Index],
				                                           SpawnParameters
				                                          );
			if (AICharacterPtr)
			{
				GroupManagger_NPCPtr->GetTeamMatesHelperComponent()->UpdateTeammateConfig(
					 AICharacterPtr->GetCharacterProxy(),
					 Index
					);
				GroupManagger_NPCPtr->AddSpwanedCharacter(AICharacterPtr);
			}
		}
	}
}

void APlanetPlayerController::ServerSpawnCharacter_Implementation(
	TSubclassOf<AHumanCharacter_AI> CharacterClass,
	const FGuid& ID,
	const FTransform& Transform,
	ETeammateOption TeammateOption
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
		Result->GetGroupManagger_NPC()->GetTeamMatesHelperComponent()->SwitchTeammateOption(TeammateOption);
	}
}

inline void APlanetPlayerController::ServerSpawnCharacterByProxyType_Implementation(
	const FGameplayTag& CharacterProxyType,
	const FTransform& Transform
	)
{
	auto ItemProxy_Description_CharacterPtr = UDataTableCollection::GetInstance()->GetTableRowProxyDescription<
		UItemProxy_Description_Character>(CharacterProxyType);

	if (ItemProxy_Description_CharacterPtr)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.CustomPreSpawnInitalization = [this](
			auto ActorPtr
			)
			{
				auto AICharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
				if (AICharacterPtr)
				{
				}
			};

		auto Result =
			GetWorld()->SpawnActor<AHumanCharacter_AI>(
			                                           ItemProxy_Description_CharacterPtr->CharacterClass,
			                                           Transform,
			                                           SpawnParameters
			                                          );
		if (Result)
		{
			// 似乎感知不到玩家？
			Result->GetGroupManagger_NPC()->GetTeamMatesHelperComponent()->AddKnowCharacter(GetPawn<ACharacterBase>());
		}
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

	auto CoinProxySPtr = InventoryComponentPtr->FindProxyType<FModifyItemProxyStrategy_Coin>(CostProxyType);
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

	CoinProxySPtr->ModifyNum(-Cost);

	if (auto IProxy_UniquePtr = DynamicCastSharedPtr<IProxy_Unique>(TargetProxySPtr))
	{
		IProxy_UniquePtr->ModifyNum(-Num);
	}
	else
	{
		TraderInventoryComponentPtr->RemoveProxy(BuyProxyID);
	}
}

void APlanetPlayerController::SwitchPlayerInput_Implementation(
	const TArray<FString>& Args
	)
{
	if (Args.IsValidIndex(0))
	{
		auto CharacterOwnerPtr = GetPawn<FPawnType>();
		if (CharacterOwnerPtr)
		{
			if (UKismetStringLibrary::Conv_StringToInt(Args[0]))
			{
				CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->AddLooseGameplayTag(
					 UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove
					);
			}
			else
			{
				CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->RemoveLooseGameplayTag(
					 UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove
					);
			}
		}
	}
}

void APlanetPlayerController::AdjustTime_Implementation(
	const FDateTime& Time
	)
{
}

void APlanetPlayerController::UpdateWeather_Implementation(
	const FGameplayTag& WeatherType
	)
{
}

void APlanetPlayerController::AddOrRemoveState_Implementation(
	const FGameplayTag& StateTag,
	bool bIsAdd
	)
{
	AddOrRemoveStateImp(StateTag, bIsAdd);
}

void APlanetPlayerController::AddOrRemoveStateImp_Implementation(
	const FGameplayTag& StateTag,
	bool bIsAdd
	)
{
	auto CharacterOwnerPtr = GetPawn<FPawnType>();
	if (CharacterOwnerPtr)
	{
		if (bIsAdd)
		{
			CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->AddLooseGameplayTag(
				 StateTag
				);
		}
		else
		{
			CharacterOwnerPtr->GetCharacterAbilitySystemComponent()->RemoveLooseGameplayTag(
				 StateTag
				);
		}
	}
}

void APlanetPlayerController::AddExperience_Implementation(
	int32 ExperienceValue
	)
{
	auto CharacterProxySPtr = GetCharacterProxy();
	if (CharacterProxySPtr)
	{
		CharacterProxySPtr->AddExperience(ExperienceValue);
	}
}

void APlanetPlayerController::UpdateCharacterTalent_Implementation(
	const FGuid& CharacterID,
	const FGameplayTag& TalentSocket,
	int32 NewLevel
	)
{
	auto TargetCharacterProxySPtr = GetGroupManagger()->GetInventoryComponent()->FindProxy<
		FModifyItemProxyStrategy_Character>(CharacterID);
	if (TargetCharacterProxySPtr)
	{
		TargetCharacterProxySPtr->UpdateTalentSocket(TalentSocket, NewLevel);
	}
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
		if (CurrentPawn && CurrentPawn->IsA(AHumanCharacter::StaticClass()))
		{
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
		}

		// GetGameplayTasksComponent()->WaitPlayerLoad();
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

			auto PossessCharacterPtr = Cast<ACharacterBase>(InPawn);
			if (PossessCharacterPtr)
			{
				MakedDamageDelegateHandle = PossessCharacterPtr->GetCharacterAbilitySystemComponent()->
				                                                 MakedDamageDelegate.AddCallback(
					                                                  std::bind(
					                                                            &ThisClass::OnPossessCharacterMakedDamage,
					                                                            this,
					                                                            std::placeholders::_1
					                                                           )
					                                                 );
			}
		}
		else if (InPawn->IsA(AHorseCharacter::StaticClass()))
		{
			auto PreviousPawnPtr = UInputProcessorSubSystem_Imp::GetInstance()->GetCurrentAction()->GetOwnerActor();

			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<HorseProcessor::FHorseRegularProcessor>(
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
		if (MakedDamageDelegateHandle)
		{
			MakedDamageDelegateHandle.Reset();
		}
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

void APlanetPlayerController::GetActorEyesViewPoint(
	FVector& Location,
	FRotator& Rotation
	) const
{
	if (!ProvideEyesViewActorSet.IsEmpty())
	{
		for (const auto& Iter : ProvideEyesViewActorSet)
		{
			if (Iter.IsValid())
			{
				Iter->GetActorEyesViewPoint(Location, Rotation);
				return;
			}
		}
	}

	Super::GetActorEyesViewPoint(Location, Rotation);
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

#if UE_EDITOR || UE_CLIENT
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
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

void APlanetPlayerController::AddProvideEyesViewActor(
	const TObjectPtr<AActor>& ProvideEyesViewActor
	)
{
	ProvideEyesViewActorSet.Add(ProvideEyesViewActor);
}

void APlanetPlayerController::RemoveProvideEyesViewActor(
	const TObjectPtr<AActor>& ProvideEyesViewActor
	)
{
	if (ProvideEyesViewActorSet.Contains(ProvideEyesViewActor))
	{
		ProvideEyesViewActorSet.Remove(ProvideEyesViewActor);
	}
}

void APlanetPlayerController::SetCharacterAttributeValue_Implementation(
	const TArray<FString>& Args
	)
{
	if (Args.Num() < 2)
	{
		return;
	}

	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr)
	{
		return;
	}

	auto GASPtr = CharacterPtr->GetCharacterAbilitySystemComponent();

	auto SpecHandle = GASPtr->MakeOutgoingSpec(
	                                           UAssetRefMap::GetInstance()->OnceGEClass,
	                                           1,
	                                           GASPtr->MakeEffectContext()
	                                          );

	SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override);

	SpecHandle.Data.Get()->SetSetByCallerMagnitude(
	                                               FGameplayTag::RequestGameplayTag(*Args[0]),
	                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
	                                              );

	const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	if (!GEHandle.IsValid())
	{
		// checkNoEntry();
	}
}

void APlanetPlayerController::ModifyElementalDataToTarget_Implementation(
	const TArray<FString>& Args
	)
{
	if (Args.Num() < 6)
	{
		return;
	}

	auto CharacterPtr = GetPawn<FPawnType>();
	if (!CharacterPtr)
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
			auto GASPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();

			auto SpecHandle = GASPtr->MakeOutgoingSpec(
			                                           UAssetRefMap::GetInstance()->OnceGEClass,
			                                           1,
			                                           GASPtr->MakeEffectContext()
			                                          );

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override);

			if (TEXT("Metal") == Args[0])
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
				                                              );
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[2])
				                                              );
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[3])
				                                              );
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[4])
				                                              );
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[5])
				                                              );
			}
			else if (TEXT("Wood") == Args[0])
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
				                                              );
			}
			else if (TEXT("Water") == Args[0])
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
				                                              );
			}
			else if (TEXT("Fire") == Args[0])
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
				                                              );
			}
			else if (TEXT("Earth") == Args[0])
			{
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
				                                              );
			}

			const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (!GEHandle.IsValid())
			{
				// checkNoEntry();
			}
		}
	}
}

void APlanetPlayerController::ModifyElementalData_Implementation(
	const TArray<FString>& Args
	)
{
	if (Args.Num() < 6)
	{
		return;
	}

	{
		auto GASPtr = GetPawn<FPawnType>()->GetCharacterAbilitySystemComponent();

		auto SpecHandle = GASPtr->MakeOutgoingSpec(
		                                           UAssetRefMap::GetInstance()->OnceGEClass,
		                                           1,
		                                           GASPtr->MakeEffectContext()
		                                          );

		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Override);

		if (TEXT("Metal") == Args[0])
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Value,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Level,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[2])
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Penetration,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[3])
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_PercentPenetration,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[4])
			                                              );
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Metal_Resistance,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[5])
			                                              );
		}
		else if (TEXT("Wood") == Args[0])
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
			                                              );
		}
		else if (TEXT("Water") == Args[0])
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
			                                              );
		}
		else if (TEXT("Fire") == Args[0])
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
			                                              );
		}
		else if (TEXT("Earth") == Args[0])
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
			                                               UKismetStringLibrary::Conv_StringToInt(Args[1])
			                                              );
		}

		const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (!GEHandle.IsValid())
		{
			// checkNoEntry();
		}
	}
}

void APlanetPlayerController::AddShieldToTarget_Implementation(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

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
			// 自动回复
			{
				auto GASPtr = GetPawn<FPawnType>()->GetCharacterAbilitySystemComponent();

				auto SpecHandle = GASPtr->MakeOutgoingSpec(
				                                           UAssetRefMap::GetInstance()->OnceGEClass,
				                                           1,
				                                           GASPtr->MakeEffectContext()
				                                          );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_Shield);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::DataSource_Character,
				                                               UKismetStringLibrary::Conv_StringToInt(Args[0])
				                                              );

				const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToTarget(
				                                                              *SpecHandle.Data.Get(),
				                                                              TargetCharacterPtr->
				                                                              GetCharacterAbilitySystemComponent()
				                                                             );
				if (!GEHandle.IsValid())
				{
					// checkNoEntry();
				}
				const auto TimerDelegate = FTimerDelegate::CreateLambda(
				                                                        [this, TargetCharacterPtr, GASPtr](
				                                                        )
				                                                        {
					                                                        auto SpecHandle = GASPtr->MakeOutgoingSpec(
						                                                         UAssetRefMap::GetInstance()->
						                                                         OnceGEClass,
						                                                         1,
						                                                         GASPtr->MakeEffectContext()
						                                                        );

					                                                        SpecHandle.Data.Get()->AddDynamicAssetTag(
						                                                         UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Data
						                                                        );
					                                                        SpecHandle.Data.Get()->AddDynamicAssetTag(
						                                                         UGameplayTagsLibrary::GEData_ModifyItem_Shield
						                                                        );
					                                                        SpecHandle.Data.Get()->
						                                                        SetSetByCallerMagnitude(
							                                                         UGameplayTagsLibrary::DataSource_Character,
							                                                         0
							                                                        );

					                                                        const auto GEHandle = GASPtr->
						                                                        ApplyGameplayEffectSpecToTarget(
							                                                         *SpecHandle.Data.Get(),
							                                                         TargetCharacterPtr->
							                                                         GetCharacterAbilitySystemComponent()
							                                                        );
					                                                        if (!GEHandle.IsValid())
					                                                        {
						                                                        // checkNoEntry();
					                                                        }
				                                                        }
				                                                       );

				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
				                                       TimerHandle,
				                                       TimerDelegate,
				                                       UKismetStringLibrary::Conv_StringToInt(Args[1]),
				                                       false
				                                      );
			}
		}
	}
}

void APlanetPlayerController::ReplyStamina_Implementation(
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
				                        UAssetRefMap::GetInstance()->OnceGEClass,
				                        1,
				                        ICPtr->MakeEffectContext()
				                       );

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);

			int32 Damege = 0;
			LexFromString(Damege, *Args[0]);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Stamina,
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

void APlanetPlayerController::AddShield_Implementation(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	// 自动回复
	{
		auto GASPtr = GetPawn<FPawnType>()->GetCharacterAbilitySystemComponent();

		auto SpecHandle = GASPtr->MakeOutgoingSpec(
		                                           UAssetRefMap::GetInstance()->OnceGEClass,
		                                           1,
		                                           GASPtr->MakeEffectContext()
		                                          );

		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data);
		SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_Shield);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(
		                                               UGameplayTagsLibrary::DataSource_Character,
		                                               UKismetStringLibrary::Conv_StringToInt(Args[0])
		                                              );

		const auto GEHandle = GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (!GEHandle.IsValid())
		{
			// checkNoEntry();
		}

		const auto TimerDelegate = FTimerDelegate::CreateLambda(
		                                                        [this](
		                                                        )
		                                                        {
			                                                        auto GASPtr = GetPawn<FPawnType>()->
				                                                        GetCharacterAbilitySystemComponent();

			                                                        auto SpecHandle = GASPtr->MakeOutgoingSpec(
				                                                         UAssetRefMap::GetInstance()->OnceGEClass,
				                                                         1,
				                                                         GASPtr->MakeEffectContext()
				                                                        );

			                                                        SpecHandle.Data.Get()->AddDynamicAssetTag(
				                                                         UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary_Data
				                                                        );
			                                                        SpecHandle.Data.Get()->AddDynamicAssetTag(
				                                                         UGameplayTagsLibrary::GEData_ModifyItem_Shield
				                                                        );
			                                                        SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                                         UGameplayTagsLibrary::DataSource_Character,
				                                                         0
				                                                        );

			                                                        const auto GEHandle = GASPtr->
				                                                        ApplyGameplayEffectSpecToSelf(
					                                                         *SpecHandle.Data.Get()
					                                                        );
			                                                        if (!GEHandle.IsValid())
			                                                        {
				                                                        // checkNoEntry();
			                                                        }
		                                                        }
		                                                       );

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
		                                       TimerHandle,
		                                       TimerDelegate,
		                                       UKismetStringLibrary::Conv_StringToInt(Args[1]),
		                                       false
		                                      );
	}
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
				                        UAssetRefMap::GetInstance()->OnceGEClass,
				                        1,
				                        ICPtr->MakeEffectContext()
				                       );

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::Proxy_Weapon_Test);

			int32 Damege = 0;
			LexFromString(Damege, *Args[0]);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal,
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
					                        UAssetRefMap::GetInstance()->OnceGEClass,
					                        1,
					                        ICPtr->MakeEffectContext()
					                       );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::Proxy_Weapon_Test);

				int32 Damege = 0;
				LexFromString(Damege, *Args[0]);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
				                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal,
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
			auto ICPtr = CharacterPtr->GetCharacterAbilitySystemComponent();
			auto ASCPtr = TargetCharacterPtr->GetCharacterAbilitySystemComponent();
			FGameplayEffectSpecHandle SpecHandle =
				ICPtr->MakeOutgoingSpec(
				                        UAssetRefMap::GetInstance()->OnceGEClass,
				                        1,
				                        ICPtr->MakeEffectContext()
				                       );

			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_BaseValue_Addtive);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Damage);
			SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::Proxy_Weapon_Test);

			int32 Damege = 0;
			LexFromString(Damege, *Args[0]);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               UGameplayTagsLibrary::GEData_ModifyItem_Damage_Metal,
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

void APlanetPlayerController::OnPossessCharacterMakedDamage(
	const FOnEffectedTargetCallback& OnEffectedTargetCallback
	)
{
	if (
		!OnEffectedTargetCallback.TargetCharacterPtr
	)
	{
		return;
	}

	const auto ProxyType = OnEffectedTargetCallback.TargetCharacterPtr->GetCharacterProxy()->GetProxyType();

	if (
		!ProxyType.MatchesTag(
		                      UGameplayTagsLibrary::Proxy_Character_NPC_Assistional
		                     )
	)
	{
		return;
	}

	if (!OnEffectedTargetCallback.bIsDeath)
	{
		return;
	}

	if (GetPlayerState<APlanetPlayerState>()->GetIsInChallenge())
	{
	}
	else
	{
		const auto TableRow_RewardsItems_DefeatEnemyPtr = UDataTableCollection::GetInstance()->
			GetTableRow_RewardsItems_DefeatEnemy(ProxyType);
		if (TableRow_RewardsItems_DefeatEnemyPtr)
		{
			FGuid Guid = FGuid::NewGuid();

			auto HICPtr = GetInventoryComponent();
			for (const auto Iter : TableRow_RewardsItems_DefeatEnemyPtr->RewardsMap)
			{
				if (Iter.Key.MatchesTag(UGameplayTagsLibrary::Proxy))
				{
					HICPtr->AddProxy_Pending(Iter.Key, Iter.Value, Guid);
				}
				else if (Iter.Key.MatchesTag(UGameplayTagsLibrary::GEData_ModifyItem_Experience))
				{
					AddExperience(Iter.Value);
				}
			}

			HICPtr->SyncPendingProxy(Guid);
		}
	}
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
