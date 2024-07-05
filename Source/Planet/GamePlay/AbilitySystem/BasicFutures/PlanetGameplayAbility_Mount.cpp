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
#include "HumanCharacter.h"

UBasicFutures_Mount::UBasicFutures_Mount() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFutures_Mount::PreActivate(
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

void UBasicFutures_Mount::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UBasicFutures_Mount::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBasicFutures_Mount::PerformAction()
{
	if (CharacterPtr)
	{
		HorseCharacterPtr->RiderPtr = Cast<AHumanCharacter>(CharacterPtr);

		CharacterPtr->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterPtr->AttachToActor(HorseCharacterPtr, FAttachmentTransformRules::KeepRelativeTransform);
		CharacterPtr->SetActorRelativeTransform(FTransform::Identity);
	}

	PlayMontage();
}

void UBasicFutures_Mount::PlayMontage()
{
	WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::OnMontageComplete));

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

void UBasicFutures_Mount::OnMontageComplete()
{
	if (CharacterPtr)
	{
		CharacterPtr->SetActorRelativeTransform(FTransform::Identity);
	}
	auto PCPtr = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PCPtr)
	{
		auto AnimInstPtr = CharacterPtr->GetAnimationIns<UHumanAnimInstance>();
		if (AnimInstPtr)
		{
			AnimInstPtr->AnimationType = EAnimationType::kMounted;
		}
		HorseCharacterPtr->SwitchDisplayMountTips(false);
		PCPtr->Possess(HorseCharacterPtr);
	}
}