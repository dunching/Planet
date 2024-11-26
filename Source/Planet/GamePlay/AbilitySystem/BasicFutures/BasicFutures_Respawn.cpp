
#include "BasicFutures_Respawn.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "HumanAIController.h"
#include "GameplayTagsSubSystem.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "HumanCharacter_Player.h"

void UBasicFutures_Respawn::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UBasicFutures_Respawn::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	PlayMontage(DeathMontage, 1.f);

	if (
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
	}
}

void UBasicFutures_Respawn::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantRotation);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_Orient2Acce);

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::State_Buff_CantBeSlected);
}

void UBasicFutures_Respawn::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			CurMontagePtr,
			Rate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontageComplete);
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontageComplete);

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_Respawn::OnMontageComplete()
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority)
		)
	{
		K2_CancelAbility();
	}

	if (
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
	}
}
