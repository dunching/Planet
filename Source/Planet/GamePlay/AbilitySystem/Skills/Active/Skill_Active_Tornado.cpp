
#include "Skill_Active_Tornado.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include <Engine/OverlapResult.h>

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
#include "AbilityTask_TimerHelper.h"
#include "Helper_RootMotionSource.h"
#include "AbilityTask_tornado.h"

ATornado::ATornado(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	CapsuleComponentPtr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponentPtr->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponentPtr->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponentPtr->CanCharacterStepUpOn = ECB_No;
	CapsuleComponentPtr->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponentPtr->SetCanEverAffectNavigation(false);
	CapsuleComponentPtr->bDynamicObstacle = true;
	CapsuleComponentPtr->SetupAttachment(RootComponent);
}

USkill_Active_Tornado::USkill_Active_Tornado() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CooldownTime = 10;
}

void USkill_Active_Tornado::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	TargetsSet.Empty();
	if (CharacterPtr)
	{
		const auto Dir = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), CharacterPtr->GetControlRotation().Vector());

		if (CharacterPtr->GetCharacterMovement()->CurrentFloor.IsWalkableFloor())
		{
			const auto Location = CharacterPtr->GetCharacterMovement()->CurrentFloor.HitResult.ImpactPoint;
			StartPt = Location + (Dir.Vector() * Offset);;
			EndPt = Location + (Dir.Vector() * (Offset + Distance));
			TornadoPtr = GetWorld()->SpawnActor<ATornado>(
				TornadoClass, StartPt, CharacterPtr->GetActorRotation()
			);
			//	TornadoPtr->CapsuleComponentPtr->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
		}
	}
}

void USkill_Active_Tornado::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	PerformAction();
}

bool USkill_Active_Tornado::CanActivateAbility(
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

void USkill_Active_Tornado::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (TornadoPtr)
	{
		TornadoPtr->Destroy();
		TornadoPtr = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Tornado::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Active_Tornado::ExcuteTasks()
{
	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->SetDuration(Duration);
	TaskPtr->SetIntervalTime(Duration);
	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnTimerHelperTick);
	TaskPtr->OnFinished.BindLambda([this](auto) {
		K2_CancelAbility();
		});
	TaskPtr->ReadyForActivation();
}

void USkill_Active_Tornado::PlayMontage()
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
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnPlayMontageEnd);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnPlayMontageEnd);

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_Tornado::OnPlayMontageEnd()
{

}

void USkill_Active_Tornado::OnTimerHelperTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval <= Interval)
	{
		const auto Percent = CurrentInterval / Interval;
		const auto NewOffset = EndPt - StartPt;
		const auto NewPt = StartPt + (Percent * NewOffset);

		TornadoPtr->SetActorLocation(NewPt);

		{
			ECollisionChannel TestChannel = ECC_MAX;
			FComponentQueryParams DefaultComponentQueryParams;
			DefaultComponentQueryParams.AddIgnoredActor(TornadoPtr);

			FCollisionObjectQueryParams DefaultObjectQueryParam;
			DefaultObjectQueryParam.AddObjectTypesToQuery(ECC_Pawn);

			TArray<FOverlapResult> OutOverlap;
			TornadoPtr->CapsuleComponentPtr->ComponentOverlapMulti(
				OutOverlap,
				GetWorld(),
				NewPt,
				CharacterPtr->GetActorRotation().Quaternion(),
				TestChannel,
				DefaultComponentQueryParams,
				DefaultObjectQueryParam
			);
			for (const auto& Iter : OutOverlap)
			{
				OnOverlap(Iter.GetActor());
			}
		}
	}
}

void USkill_Active_Tornado::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	OnOverlap(OtherActor);
}

void USkill_Active_Tornado::OnOverlap(AActor* OtherActor)
{
	if (OtherActor && OtherActor->IsA(ACharacterBase::StaticClass()))
	{
		auto OtherCharacterPtr = Cast<ACharacterBase>(OtherActor);
		if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
		{
			return;
		}

		// 
		if (!TargetsSet.Contains(OtherCharacterPtr))
		{
			TargetsSet.Add(OtherCharacterPtr);

			auto TaskPtr = UAbilityTask_Tornado::TornadoTask(
				this,
				TEXT(""),
				TornadoPtr,
				OtherCharacterPtr
			);

			TaskPtr->ReadyForActivation();
		}
	}
}