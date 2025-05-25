
#include "BasicFutures_Respawn.h"

#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "AbilityTask_PlayMontage.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "HumanAIController.h"
#include "GameplayTagsLibrary.h"
#include "HumanCharacter_AI.h"
#include "HumanRegularProcessor.h"
#include "InputProcessorSubSystem.h"
#include "HumanCharacter_Player.h"
#include "Teleport.h"

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
	//
	// TArray<AActor*> OutActors;
	// UGameplayStatics::GetAllActorsOfClass(this, ATeleport::StaticClass(), OutActors);
	// for (auto Iter :OutActors)
	// {
	// 	TeleportPtr = Cast<ATeleport>(Iter);
	// 	if (TeleportPtr)
	// 	{
	// 		break;
	// 	}
	// }
	//
	// if (TeleportPtr)
	// {
	// 	CharacterPtr->TeleportTo();
	// }
	PlayMontage(DeathMontage, 1.f);

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
	}
}

// void UBasicFutures_Respawn::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
//
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantJump);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRootMotion);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRotation);
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_Orient2Acce);
//
// 	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Buff_CantBeSlected);
// }

void UBasicFutures_Respawn::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			CurMontagePtr,
			Rate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontageComplete);
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::OnMontageComplete);

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_Respawn::OnMontageComplete()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		)
	{
		// 清除周围AI对这个Character得感知，以达到重新感知此Character
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(this, AHumanCharacter_AI::StaticClass(),OutActors );
		for (auto Iter : OutActors)
		{
			auto HumanCharacter_AIPtr =  Cast<AHumanCharacter_AI>(Iter);
			if (HumanCharacter_AIPtr)
			{
				HumanCharacter_AIPtr->GetController<AHumanAIController>()->GetAIPerceptionComponent()->ForgetActor(CharacterPtr);
			}
		}
		
		K2_CancelAbility();
	}

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
		)
	{
		UInputProcessorSubSystem::GetInstance()->SwitchToProcessor<HumanProcessor::FHumanRegularProcessor>();
	}
}
