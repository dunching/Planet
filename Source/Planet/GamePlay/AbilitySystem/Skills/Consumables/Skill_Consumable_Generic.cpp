
#include "Skill_Consumable_Generic.h"

#include "AbilitySystemComponent.h"
#include <Components/SkeletalMeshComponent.h>

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GAEvent_Helper.h"
#include "Consumable_Test.h"
#include "InteractiveBaseGAComponent.h"

void USkill_Consumable_Generic::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_Consumable_Generic::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Consumable_Generic*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			UnitPtr = GameplayAbilityTargetDataPtr->UnitPtr;
		}
	}
}

void USkill_Consumable_Generic::ActivateAbility(
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

bool USkill_Consumable_Generic::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{


	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool USkill_Consumable_Generic::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Consumable_Generic::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Consumable_Generic::PerformAction()
{
	if (CharacterPtr)
	{
		SpawnActor();
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Consumable_Generic::SpawnActor()
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = CharacterPtr;
	ConsumableActorPtr = GetWorld()->SpawnActor<AConsumable_Test>(Consumable_Class, ActorSpawnParameters);
	if (ConsumableActorPtr)
	{
		ConsumableActorPtr->Interaction(CharacterPtr);
	}
}

void USkill_Consumable_Generic::ExcuteTasks()
{
	if (CharacterPtr)
	{
		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		auto GAPtr = ICPtr->ExcuteEffects(UnitPtr);
	}
}

void USkill_Consumable_Generic::PlayMontage()
{
	if (UnitPtr->HumanMontage)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			UnitPtr->HumanMontage,
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

void USkill_Consumable_Generic::OnPlayMontageEnd()
{
	if (ConsumableActorPtr)
	{
		ConsumableActorPtr->Destroy();
		ConsumableActorPtr = nullptr;
	}

	K2_CancelAbility();
}

void USkill_Consumable_Generic::EmitEffect()
{
}

void USkill_Consumable_Generic::OnGAEnd(UGameplayAbility* GAPtr)
{
}
