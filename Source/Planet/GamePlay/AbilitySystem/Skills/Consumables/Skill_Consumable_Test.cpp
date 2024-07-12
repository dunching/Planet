
#include "Skill_Consumable_Test.h"

#include "AbilitySystemComponent.h"
#include <Components/SkeletalMeshComponent.h>

#include "AbilityTask_PlayMontage.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GAEvent_Helper.h"
#include "Consumable_Test.h"

void USkill_Consumable_Test::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_Consumable_Test::PerformAction()
{
	if (CharacterPtr)
	{
		SpawnActor();
		ExcuteTasks();
		PlayMontage();
	}
}

void USkill_Consumable_Test::SpawnActor()
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = CharacterPtr;
	ConsumableActorPtr = GetWorld()->SpawnActor<AConsumable_Test>(Consumable_Class, ActorSpawnParameters);
	if (ConsumableActorPtr)
	{
		ConsumableActorPtr->Interaction(CharacterPtr);
	}
}

void USkill_Consumable_Test::ExcuteTasks()
{
	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->SetDuration(Duration, PerformActionInterval);
	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnTimerHelperTick);
	TaskPtr->OnFinished.BindLambda([this](auto) {
		K2_CancelAbility();
		});
	TaskPtr->ReadyForActivation();
}

void USkill_Consumable_Test::OnTimerHelperTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
		EmitEffect();
	}
}

void USkill_Consumable_Test::PlayMontage()
{
	if (HumanMontage)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
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

void USkill_Consumable_Test::OnPlayMontageEnd()
{
	if (ConsumableActorPtr)
	{
		ConsumableActorPtr->Destroy();
		ConsumableActorPtr = nullptr;
	}
}

void USkill_Consumable_Test::EmitEffect()
{
	FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

	FGameplayEventData Payload;
	Payload.TargetData.Add(GAEventDataPtr);

	GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

	FGAEventData GAEventData(CharacterPtr, CharacterPtr);

	GAEventData.HP = HP;

	GAEventDataPtr->DataAry.Add(GAEventData);

	SendEvent(Payload);
}

void USkill_Consumable_Test::PreActivate(
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
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Consumable_Test*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			UnitPtr = GameplayAbilityTargetDataPtr->UnitPtr;
		}
	}
}

void USkill_Consumable_Test::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}
