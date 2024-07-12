
#include "Skill_Consumable_Generic.h"

#include "AbilitySystemComponent.h"
#include <Components/SkeletalMeshComponent.h>

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GAEvent_Helper.h"
#include "Consumable_Test.h"

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
	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->SetDuration(UnitPtr->Duration, UnitPtr->PerformActionInterval);
	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
	TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
	TaskPtr->OnFinished.BindLambda([this](auto) {
		K2_CancelAbility();
		});
	TaskPtr->ReadyForActivation();
}

void USkill_Consumable_Generic::OnInterval(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
		EmitEffect();
	}
}

void USkill_Consumable_Generic::OnDuration(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{

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
}

void USkill_Consumable_Generic::EmitEffect()
{
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

	FGameplayEventData Payload;
	Payload.TargetData.Add(GAEventDataPtr);

	GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

	FGAEventData GAEventData(CharacterPtr, CharacterPtr);

	for (const auto& Iter : UnitPtr->ModifyPropertyMap)
	{
		switch (Iter.Key)
		{
		case ECharacterPropertyType::kHP:
		{
			GAEventData.HP = Iter.Value.GetCurrentValue();
		}
		break;
		}
	}

	GAEventDataPtr->DataAry.Add(GAEventData);

	SendEvent(Payload);
}
