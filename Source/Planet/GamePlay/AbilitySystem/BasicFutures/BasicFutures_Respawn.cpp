
#include "BasicFutures_Respawn.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "HumanAIController.h"
#include "GameplayTagsSubSystem.h"
#include "Planet_Tools.h"

void UBasicFutures_Respawn::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CancelAbilitiesWithTag.AddTag(UGameplayTagsSubSystem::GetInstance()->DeathingTag);
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

	if (auto AIPCPtr = CharacterPtr->GetController<AHumanAIController>())
	{

	}
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

		TaskPtr->ReadyForActivation();
	}
}
