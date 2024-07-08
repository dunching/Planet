
#include "BasicFutures_Death.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "AbilityTask_PlayMontage.h"

void UBasicFutures_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WaitingToExecute.Add(FPostLockDelegate::CreateLambda([this]() {
		K2_EndAbility();
		}));

	PlayMontage(DeathMontage, 1.f);
}

void UBasicFutures_Death::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	{
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			CurMontagePtr,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLockOverride);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLockOverride);

		TaskPtr->ReadyForActivation();

		IncrementListLock();
	}
}
