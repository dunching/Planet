// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BasicFutures_DisMount.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Components/CapsuleComponent.h>

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "ProxyProcessComponent.h"

#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "HumanAIController.h"
#include "HorseCharacter.h"
#include "HumanCharacter.h"
#include "HumanAnimInstance.h"

UBasicFutures_DisMount::UBasicFutures_DisMount() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBasicFutures_DisMount::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, 
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_DisMount*>(TriggerEventData->TargetData.Get(0));
	if (GameplayAbilityTargetDataPtr)
	{
		HorseCharacterPtr = GameplayAbilityTargetDataPtr->HorseCharacterPtr;
	}

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UBasicFutures_DisMount::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Perform();
}

void UBasicFutures_DisMount::EndAbility(
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

void UBasicFutures_DisMount::Perform()
{
	auto AnimInstPtr = CharacterPtr->GetAnimationIns<UHumanAnimInstance>();
	if (AnimInstPtr)
	{
		AnimInstPtr->AnimationType = EAnimationType::kNormal;
	}

	CharacterPtr->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HorseCharacterPtr->RiderPtr = nullptr;

	PlayMontage();
}

void UBasicFutures_DisMount::PlayMontage()
{
	WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::OnMontageComplete));

	auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
		this,
		TEXT(""),
		MontagePtr,
		1.f
	);

	TaskPtr->Ability = this;
	TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

	TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
	TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

	TaskPtr->ReadyForActivation();

	IncrementListLock();
}

void UBasicFutures_DisMount::OnMontageComplete()
{
	auto PCPtr = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PCPtr)
	{
		PCPtr->Possess(CharacterPtr);
	}
	CharacterPtr->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	K2_EndAbility();
}
