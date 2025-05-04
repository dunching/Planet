#include "PlanetAIController.h"

#include "Components/StateTreeComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "TeamMatesHelperComponent.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "GameplayTagsLibrary.h"
#include "KismetGravityLibrary.h"
#include "GroupManagger.h"
#include "LogWriter.h"
#include "Components/SplineComponent.h"

APlanetAIController::APlanetAIController(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void APlanetAIController::OnPossess(
	APawn* InPawn
	)
{
	bool bIsNewPawn = (InPawn && InPawn != GetPawn());

	if (bIsNewPawn)
	{
	}

	Super::OnPossess(InPawn);

	if (bIsNewPawn)
	{
		if (InPawn)
		{
			BindPCWithCharacter();
		}
	}
}

UPlanetAbilitySystemComponent* APlanetAIController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAbilitySystemComponent();
}

void APlanetAIController::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GroupManaggerPtr, COND_None);
}

AGroupManagger* APlanetAIController::GetGroupManagger() const
{
	return GroupManaggerPtr;
}

void APlanetAIController::SetGroupSharedInfo(
	AGroupManagger* InGroupSharedInfoPtr
	)
{
	GroupManaggerPtr = InGroupSharedInfoPtr;

	OnGroupManaggerReady(GroupManaggerPtr);
}

UInventoryComponent* APlanetAIController::GetInventoryComponent() const
{
	return GetPawn<FPawnType>()->GetInventoryComponent();
}

UCharacterAttributesComponent* APlanetAIController::GetCharacterAttributesComponent() const
{
	return GetPawn<FPawnType>()->GetCharacterAttributesComponent();
}

UTalentAllocationComponent* APlanetAIController::GetTalentAllocationComponent() const
{
	return GetPawn<FPawnType>()->GetTalentAllocationComponent();
}

TSharedPtr<FCharacterProxy> APlanetAIController::GetCharacterProxy()
{
	return GetPawn<FPawnType>()->GetCharacterProxy();
}

ACharacterBase* APlanetAIController::GetRealCharacter() const
{
	return Cast<ACharacterBase>(GetPawn());
}

void APlanetAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APlanetAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 
}

void APlanetAIController::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

TWeakObjectPtr<ACharacterBase> APlanetAIController::GetTeamFocusTarget() const
{
	if (GetGroupManagger() && GetGroupManagger()->GetTeamMatesHelperComponent())
	{
		return GetGroupManagger()->GetTeamMatesHelperComponent()->GetForceKnowCharater();
	}

	return nullptr;
}

bool APlanetAIController::CheckIsFarawayOriginal() const
{
	return false;
}

void APlanetAIController::ResetGroupmateProxy(
	FCharacterProxy* NewGourpMateProxyPtr
	)
{
}

void APlanetAIController::BindPCWithCharacter()
{
}

TSharedPtr<FCharacterProxy> APlanetAIController::InitialCharacterProxy(
	ACharacterBase* CharaterPtr
	)
{
	return CharaterPtr->GetCharacterProxy();
}

void APlanetAIController::OnGroupManaggerReady(
	AGroupManagger* NewGroupSharedInfoPtr
	)
{
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

void APlanetAIController::OnRep_GroupSharedInfoChanged()
{
	OnGroupManaggerReady(GroupManaggerPtr);
}

void APlanetAIController::OnTargetPerceptionUpdated(
	AActor* Actor,
	FAIStimulus Stimulus
	)
{
	auto CharacterPtr = Cast<AHumanCharacter>(Actor);
	if (CharacterPtr)
	{
		if (IsGroupmate(CharacterPtr) || CharacterPtr->GetCharacterAttributesComponent()->CharacterCategory.MatchesTag(
			     UGameplayTagsLibrary::Proxy_Character_NPC_Functional
			    ))
		{
			return;
		}
		else
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				GetGroupManagger()->GetTeamMatesHelperComponent()->AddKnowCharacter(CharacterPtr);
			}
			else
			{
				GetGroupManagger()->GetTeamMatesHelperComponent()->RemoveKnowCharacter(CharacterPtr);
			}
		}
	}
}

void APlanetAIController::OnPerceptionUpdated(
	const TArray<AActor*>& UpdatedActors
	)
{
}

bool APlanetAIController::IsGroupmate(
	ACharacterBase* TargetCharacterPtr
	) const
{
	return GetPawn<FPawnType>()->IsGroupmate(TargetCharacterPtr);
}

bool APlanetAIController::IsTeammate(
	ACharacterBase* TargetCharacterPtr
	) const
{
	return GetPawn<FPawnType>()->IsTeammate(TargetCharacterPtr);
}

void APlanetAIController::OnHPChanged(
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
			                                                      for (auto Iter : AbilityEndedData.AbilityThatEnded->
			                                                           GetAssetTags())
			                                                      {
				                                                      if (Iter == UGameplayTagsLibrary::State_Dying)
				                                                      {
					                                                      //					Destroy();
				                                                      }
			                                                      }
		                                                      }
		                                                     );
	}
}

FPathFollowingRequestResult APlanetAIController::MoveAlongSPline(
	USplineComponent* SplineComponentPtr,
	const FAIMoveRequest& MoveRequest,
	FNavPathSharedPtr* OutPath
	)
{
	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	TArray<FVector> Points;
	const auto Len = SplineComponentPtr->GetSplineLength();
	for (int32 CurrentLen = 0; CurrentLen < Len; CurrentLen += 100)
	{
		auto Pt = SplineComponentPtr->GetLocationAtDistanceAlongSpline(CurrentLen, ESplineCoordinateSpace::World);

		FNavLocation OutLocation;
		if (NavSys->ProjectPointToNavigation(Pt, OutLocation))
		{
			Points.Add(OutLocation.Location);
		}
		else
		{
		}
	}

	FNavPathSharedPtr Path = MakeShared<FNavigationPath>(Points);

	const FAIRequestID RequestID = Path.IsValid() ? RequestMove(MoveRequest, Path) : FAIRequestID::InvalidRequest;
	if (RequestID.IsValid())
	{
		bAllowStrafe = MoveRequest.CanStrafe();
		ResultData.MoveId = RequestID;
		ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

		if (OutPath)
		{
			*OutPath = Path;
		}
	}

	return ResultData;
}

void APlanetAIController::LimitViewYaw(
	FRotator& ViewRotation,
	float InViewYawMin,
	float InViewYawMax
	)
{
	ViewRotation.Yaw = FMath::ClampAngle(ViewRotation.Yaw, InViewYawMin, InViewYawMax);
	ViewRotation.Yaw = FRotator::ClampAxis(ViewRotation.Yaw);
}
