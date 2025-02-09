
#include "PlanetAIController.h"

#include "Components/StateTreeComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "TeamMatesHelperComponent.h"
#include "ItemProxy_Minimal.h"
#include "HumanCharacter.h"
#include "GameplayTagsLibrary.h"
#include "KismetGravityLibrary.h"
#include "GroupSharedInfo.h"
#include "LogWriter.h"

APlanetAIController::APlanetAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void APlanetAIController::OnPossess(APawn* InPawn)
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

void APlanetAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GroupSharedInfoPtr, COND_None);
}

AGroupSharedInfo* APlanetAIController::GetGroupSharedInfo() const
{
	return GroupSharedInfoPtr;
}

void APlanetAIController::SetGroupSharedInfo(AGroupSharedInfo* InGroupSharedInfoPtr)
{
	GroupSharedInfoPtr = InGroupSharedInfoPtr;

	OnGroupSharedInfoReady(GroupSharedInfoPtr);
}

UInventoryComponent* APlanetAIController::GetHoldingItemsComponent() const
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

ACharacterBase* APlanetAIController::GetRealCharacter()const
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
	InitialGroupSharedInfo();
}

void APlanetAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlanetAIController::InitialGroupSharedInfo()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (GetInstigator()->GetParentActor())
		{
		}
		// 如果这个Character是单独的，则直接生成 
		else
		{
			FActorSpawnParameters SpawnParameters;

			SpawnParameters.Owner = this;
			SpawnParameters.CustomPreSpawnInitalization = [](AActor* ActorPtr)
			{
				PRINTINVOKEINFO();
				auto GroupSharedInfoPtr = Cast<AGroupSharedInfo>(ActorPtr);
				if (GroupSharedInfoPtr)
				{
					GroupSharedInfoPtr->GroupID = FGuid::NewGuid();
				}
			};

			GroupSharedInfoPtr = GetWorld()->SpawnActor<AGroupSharedInfo>(
				AGroupSharedInfo::StaticClass(), SpawnParameters
			);

			OnGroupSharedInfoReady(GroupSharedInfoPtr);
		}
	}
#endif
}

TWeakObjectPtr<ACharacterBase> APlanetAIController::GetTeamFocusTarget() const
{
	if (GetGroupSharedInfo() && GetGroupSharedInfo()->GetTeamMatesHelperComponent())
	{
		return GetGroupSharedInfo()->GetTeamMatesHelperComponent()->GetKnowCharacter();
	}

	return nullptr;
}

bool APlanetAIController::CheckIsFarawayOriginal() const
{
	return false;
}

void APlanetAIController::ResetGroupmateProxy(FCharacterProxy* NewGourpMateProxyPtr)
{
}

void APlanetAIController::BindPCWithCharacter()
{
}

TSharedPtr<FCharacterProxy> APlanetAIController::InitialCharacterProxy(ACharacterBase* CharaterPtr)
{
	return CharaterPtr->GetCharacterProxy();
}

void APlanetAIController::OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr)
{
	ForEachComponent(false, [this](UActorComponent*ComponentPtr)
	{
		auto GroupSharedInterfacePtr = Cast<IGroupSharedInterface>(ComponentPtr);
		if (GroupSharedInterfacePtr)
		{
			GroupSharedInterfacePtr->OnGroupSharedInfoReady(GroupSharedInfoPtr);
		}
	});
}

void APlanetAIController::OnRep_GroupSharedInfoChanged()
{
	OnGroupSharedInfoReady(GroupSharedInfoPtr);
}

void APlanetAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	auto CharacterPtr = Cast<AHumanCharacter>(Actor);
	if (CharacterPtr)
	{
		if (IsGroupmate(CharacterPtr))
		{
			return;
		}
		else
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				GetGroupSharedInfo()->GetTeamMatesHelperComponent()->AddKnowCharacter(CharacterPtr);
			}
			else
			{
				GetGroupSharedInfo()->GetTeamMatesHelperComponent()->RemoveKnowCharacter(CharacterPtr);
			}
		}
	}
}

void APlanetAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
}

bool APlanetAIController::IsGroupmate(ACharacterBase* TargetCharacterPtr) const
{
	return GetPawn<FPawnType>()->IsGroupmate(TargetCharacterPtr);
}

bool APlanetAIController::IsTeammate(ACharacterBase* TargetCharacterPtr) const
{
	return GetPawn<FPawnType>()->IsTeammate(TargetCharacterPtr);
}

void APlanetAIController::OnHPChanged(int32 CurrentValue)
{
	if (CurrentValue <= 0)
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer{ UGameplayTagsLibrary::DeathingTag });
		GetAbilitySystemComponent()->OnAbilityEnded.AddLambda([this](const FAbilityEndedData& AbilityEndedData) {
			for (auto Iter : AbilityEndedData.AbilityThatEnded->AbilityTags)
			{
				if (Iter == UGameplayTagsLibrary::DeathingTag)
				{
//					Destroy();
				}
			}
			});
	}
}

void APlanetAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator DeltaRot = FRotator::ZeroRotator;
		FRotator ViewRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			const auto Z = -UKismetGravityLibrary::GetGravity();
			const auto FocusRotation = UKismetMathLibrary::Quat_FindBetweenVectors(
				UKismetMathLibrary::MakeRotFromZX(Z, ViewRotation.Vector()).Vector(),
				UKismetMathLibrary::MakeRotFromZX(Z, FocalPoint - MyPawn->GetActorLocation()).Vector()
			).Rotator();

			const float AngleTolerance = 1e-3f;
			if (FMath::IsNearlyZero(FocusRotation.Yaw, AngleTolerance))
			{
				DeltaRot.Yaw = 0.f;
			}
			else
			{
				const float RotationRate_Yaw = 120.f * DeltaTime;
				DeltaRot.Yaw = FMath::FixedTurn(0.f, FocusRotation.Yaw, RotationRate_Yaw);

				ViewRotation += DeltaRot;
				DeltaRot = FRotator::ZeroRotator;
			}
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			ViewRotation = MyPawn->GetActorRotation();
		}

		LimitViewYaw(ViewRotation, 0.f, 360.f - 0.1f);
		ViewRotation.Pitch = 0.f;
		ViewRotation.Roll = 0.f;

		SetControlRotation(ViewRotation);

		if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(ViewRotation, 1e-3f) == false)
			{
#if WITH_EDITOR
				RootComponent->SetWorldLocation(MyPawn->GetActorLocation());
#endif

				// 不要直接使用Controller上的旋转
				// MyPawn->FaceRotation(ViewRotation, DeltaTime);
			}
		}
	}
}

void APlanetAIController::LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax)
{
	ViewRotation.Yaw = FMath::ClampAngle(ViewRotation.Yaw, InViewYawMin, InViewYawMax);
	ViewRotation.Yaw = FRotator::ClampAxis(ViewRotation.Yaw);
}
