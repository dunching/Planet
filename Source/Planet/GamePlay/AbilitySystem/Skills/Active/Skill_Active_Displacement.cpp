
#include "Skill_Active_Displacement.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"

USkill_Active_Displacement::USkill_Active_Displacement() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Active_Displacement::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_Displacement::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

bool USkill_Active_Displacement::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return true;
	}
	return false;
}

void USkill_Active_Displacement::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (SPlineActorPtr)
	{
		SPlineActorPtr->Destroy();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Displacement::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CharacterPtr)
	{
		FindTarget();
		PlayMontage();
	}
}

void USkill_Active_Displacement::FindTarget()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionShape  CollisionShape = FCollisionShape::MakeSphere(200);

	FCollisionQueryParams CapsuleParams;
	CapsuleParams.AddIgnoredActor(CharacterPtr);

	TArray<struct FHitResult> OutHits;

	ACharacterBase* TargetCharacterPtr = nullptr;
	if (GetWorldImp()->SweepMultiByObjectType(
		OutHits,
		CharacterPtr->GetActorLocation(),
		CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * Distance),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape,
		CapsuleParams
	))
	{
		for (auto Iter : OutHits)
		{
			TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				break;
			}
		}
	}

	FVector TargetPt = FVector::ZeroVector;

	const auto Pt1 = CharacterPtr->GetActorLocation();
	if (TargetCharacterPtr)
	{
		const FVector Offset = (CharacterPtr->GetActorLocation() - TargetCharacterPtr->GetActorLocation()).GetSafeNormal() * ToCharacterOffset;
		TargetPt = TargetCharacterPtr->GetActorLocation() + Offset;
	}
	else
	{
		TargetPt = CharacterPtr->GetActorLocation() + (CharacterPtr->GetActorForwardVector() * Distance);
	}

	const auto Pt2 = TargetPt;
	const auto Pt3 = Pt1 + ((Pt2 - Pt1) / 2);

	const auto MidPt = Pt3 - (CharacterPtr->GetGravityDirection() * 100.f);

	SPlineActorPtr = GetWorldImp()->SpawnActor<ASPlineActor>(Pt1, FRotator::ZeroRotator);

	SPlineActorPtr->SplineComponentPtr->ClearSplinePoints();

	SPlineActorPtr->SplineComponentPtr->AddSplinePoint(Pt1, ESplineCoordinateSpace::World);
	SPlineActorPtr->SplineComponentPtr->AddSplinePoint(MidPt, ESplineCoordinateSpace::World);
	SPlineActorPtr->SplineComponentPtr->AddSplinePoint(Pt2, ESplineCoordinateSpace::World);

	SPlineActorPtr->SplineComponentPtr->SetTangentsAtSplinePoint(0, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::World);

	SPlineActorPtr->SplineComponentPtr->SetTangentsAtSplinePoint(2, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::World);

	auto TaskPtr = UAbilityTask_ApplyRootMotionBySPline::NewTask(
		this,
		TEXT(""),
		Duration,
		SPlineActorPtr,
		CharacterPtr
	);
	TaskPtr->OnFinish.BindUObject(this, &ThisClass::K2_CancelAbility);
	TaskPtr->ReadyForActivation();
}

void USkill_Active_Displacement::PlayMontage()
{
	{
		const float InPlayRate = HumanMontage->CalculateSequenceLength() / Duration;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		TaskPtr->ReadyForActivation();
	}
}
