
#include "BasicFutures_Death.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "HumanAIController.h"
#include "GameplayTagsLibrary.h"
#include "Planet_Tools.h"
#include "HumanEndangeredProcessor.h"
#include "InputProcessorSubSystem.h"
#include "HumanCharacter_Player.h"

void UBasicFutures_Death::ActivateAbility(
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

	if (
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanEndangeredProcessor>();
	}
}

void UBasicFutures_Death::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRotation);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_Orient2Acce);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Buff_CantBeSlected);
}

void UBasicFutures_Death::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
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

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_Death::OnMontageComplete()
{
	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) 
		)
	{
	}
}
