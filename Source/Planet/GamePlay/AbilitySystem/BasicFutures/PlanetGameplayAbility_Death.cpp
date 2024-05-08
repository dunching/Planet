
#include "PlanetGameplayAbility_Death.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"

void UPlanetGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	WaitingToExecute.Add(FPostLockDelegate::CreateLambda([this]() {
		K2_EndAbility();
		}));
	PlayMontage(DeathMontage, 1.f);
}

void UPlanetGameplayAbility_Death::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	{
		auto TaskPtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			CurMontagePtr,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::DecrementListLock);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::DecrementListLock);

		TaskPtr->ReadyForActivation();

		IncrementListLock();
	}
}
