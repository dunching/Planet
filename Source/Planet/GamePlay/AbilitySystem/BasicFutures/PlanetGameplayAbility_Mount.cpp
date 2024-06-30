// Copyright 2020 Dan Kestranek.

#include "PlanetGameplayAbility_Mount.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "HumanAIController.h"
#include "HorseCharacter.h"
#include "HumanAnimInstance.h"

UPlanetGameplayAbility_Mount::UPlanetGameplayAbility_Mount() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlanetGameplayAbility_Mount::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, 
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Mount*>(TriggerEventData->TargetData.Get(0));
	if (GameplayAbilityTargetDataPtr)
	{
		HorseCharacterPtr = GameplayAbilityTargetDataPtr->HorseCharacterPtr;
	}

	 CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UPlanetGameplayAbility_Mount::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayMontage();
}

void UPlanetGameplayAbility_Mount::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (CharacterPtr)
	{
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPlanetGameplayAbility_Mount::Mount()
{
	auto PCPtr = UGameplayStatics::GetPlayerController(GetWorld(), 0); 
	if (PCPtr)
	{
		CharacterPtr->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		auto AnimInstPtr = CharacterPtr->GetAnimationIns<UHumanAnimInstance>();
		if (AnimInstPtr)
		{
			AnimInstPtr->AnimationType = EAnimationType::kMounted;
		}

		PCPtr->Possess(HorseCharacterPtr);

		CharacterPtr->AttachToActor(HorseCharacterPtr, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
}

void UPlanetGameplayAbility_Mount::PlayMontage()
{
	WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::Mount));

	auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
		this,
		TEXT(""),
		MontagePtr,
		CharacterPtr->GetMesh()->GetAnimInstance(),
		1.f
	);

	TaskPtr->Ability = this;
	TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

	TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
	TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

	TaskPtr->ReadyForActivation();

	IncrementListLock();
}
