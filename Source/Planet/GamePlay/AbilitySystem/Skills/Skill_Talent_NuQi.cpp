
#include "Skill_Talent_NuQi.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"

USkill_Talent_NuQi::USkill_Talent_NuQi() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_Talent_NuQi::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle = CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.AddLambda([this, Spec](UGameplayAbility* GAPtr) {
			auto CharacterPtr = Cast<ACharacterBase>(GAPtr->GetActorInfo().AvatarActor.Get());
			if (CharacterPtr)
			{
				if (
					GAPtr &&
					(GAPtr->GetCurrentAbilitySpecHandle() == CharacterPtr->GetEquipmentItemsComponent()->SendEventHandle)
					)
				{
					AddNuQi();
				}
			}
			});

		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		OnValueChanged = CharacterAttributes.HP.GetCurrentProperty().CallbackContainerHelper.AddOnValueChanged(
			std::bind(&ThisClass::OnHPValueChanged, this, std::placeholders::_1, std::placeholders::_2)
		);
	}
}

void USkill_Talent_NuQi::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (IsMarkPendingKillOnAbilityEnd())
	{
		if (OnValueChanged)
		{
			OnValueChanged->UnBindCallback();
		}

		if (CharacterPtr)
		{
			CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(AbilityActivatedCallbacksHandle);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Talent_NuQi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInFury)
	{
		FuryDuration_Accumulate += DeltaTime;
		if (FuryDuration_Accumulate >= (FuryDuration + MaxExtendDuration_Accumulate))
		{
			StopFuryState();
			StartForceWeakState();
		}
	}
	else 
	{
		Tick_Accumulate += DeltaTime;
		if (Tick_Accumulate >= Tick_Interval)
		{
			Tick_Accumulate = 0.f;

			SubNuQi(DeltaTime);
		}

		if (bIsInWeak)
		{
			WeakDuration_Accumulate += DeltaTime;
			if (WeakDuration_Accumulate >= WeakDuration)
			{
				StopForceWeakState();
			}
		}
	}

	if (EffectItemPtr)
	{
		if (bIsInFury)
		{
			EffectItemPtr->SetPercent(true, 1.f - (FuryDuration_Accumulate / (FuryDuration + MaxExtendDuration_Accumulate)));
		}
		else if (bIsInWeak)
		{
			EffectItemPtr->SetPercent(true, 1.f - (WeakDuration_Accumulate / WeakDuration));
		}
	}
}

void USkill_Talent_NuQi::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USkill_Talent_NuQi::AddNuQi()
{
	DecrementTime_Accumulate = 0.f;

	if (bIsInFury || bIsInWeak)
	{

	}
	else
	{
		auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		CharacterAttributes.NuQi.AddCurrentValue(AttackIncrement);
		if (CharacterAttributes.NuQi.GetCurrentValue() >= 100)
		{
			StartFuryState();
		}
	}
}

void USkill_Talent_NuQi::SubNuQi(float Inveral)
{
	if (bIsInWeak)
	{
		auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		const auto Value = CharacterAttributes.NuQi.GetMaxValue() / FMath::FloorToFloat(WeakDuration);
		CharacterAttributes.NuQi.AddCurrentValue(-Value);
	}
	else
	{
		DecrementTime_Accumulate += Inveral;
		if (DecrementTime_Accumulate > DecrementTime)
		{
			auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			CharacterAttributes.NuQi.AddCurrentValue(-Decrement);
		}
	}
}

void USkill_Talent_NuQi::OnHPValueChanged(int32 OldValue, int32 NewValue)
{
	if (NewValue < OldValue)
	{
		AddNuQi();
	}
}

void USkill_Talent_NuQi::StartFuryState()
{
	bIsInFury = true;

	FuryDuration_Accumulate = 0.f;
	MaxExtendDuration_Accumulate = 0.f;

	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddCurrentValue(55);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_Penetration.AddCurrentValue(100);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_PercentPenetration.AddCurrentValue(50);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().WalkingSpeed.AddCurrentValue(100);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.AddCurrentValue(100);
	}

	auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
	if (EffectPtr)
	{
		EffectItemPtr = EffectPtr->AddEffectItem();
		EffectItemPtr->SetTexutre(FuryIcon);
	}
}

void USkill_Talent_NuQi::StopFuryState()
{
	bIsInFury = false;

	if (CharacterPtr)
	{
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD.AddCurrentValue(-55);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_Penetration.AddCurrentValue(-100);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().AD_PercentPenetration.AddCurrentValue(-50);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().WalkingSpeed.AddCurrentValue(-100);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.AddCurrentValue(-100);
	}

	if (EffectItemPtr)
	{
		EffectItemPtr->RemoveFromParent();
		EffectItemPtr = nullptr;
	}
}

void USkill_Talent_NuQi::StartForceWeakState()
{
	bIsInWeak = true;

	WeakDuration_Accumulate = 0.f;

	auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
	if (EffectPtr)
	{
		EffectItemPtr = EffectPtr->AddEffectItem();
		EffectItemPtr->SetTexutre(WeakIcon);
	}
}

void USkill_Talent_NuQi::StopForceWeakState()
{
	bIsInWeak = false;

	if (EffectItemPtr)
	{
		EffectItemPtr->RemoveFromParent();
		EffectItemPtr = nullptr;
	}
}