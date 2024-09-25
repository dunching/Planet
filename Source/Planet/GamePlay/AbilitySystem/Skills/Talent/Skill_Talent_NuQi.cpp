
#include "Skill_Talent_NuQi.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "BaseFeatureComponent.h"

int32 FTalent_NuQi::GetCurrentValue() const
{
	return CurrentValue;
}

int32 FTalent_NuQi::GetMaxValue() const
{
	return MaxValue;
}

void FTalent_NuQi::SetCurrentValue(int32 NewVal)
{
	if (CurrentValue != NewVal)
	{
		NewVal = FMath::Clamp(NewVal, 0, MaxValue);

		CallbackContainerHelper.ValueChanged(CurrentValue, NewVal);

		CurrentValue = NewVal;
	}
}

void FTalent_NuQi::AddCurrentValue(int32 Value)
{
	SetCurrentValue(GetCurrentValue() + Value);
}

USkill_Talent_NuQi::USkill_Talent_NuQi() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_Talent_NuQi::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle = CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnSendDamage);

		auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		OnValueChanged = CharacterAttributes.HP.AddOnValueChanged(
			std::bind(&ThisClass::OnHPValueChanged, this, std::placeholders::_1, std::placeholders::_2)
		);

		TalentSPtr = TSharedPtr<FCurrentTalentType>(
			CharacterAttributes.TalentSPtr, dynamic_cast<FCurrentTalentType*>(CharacterAttributes.TalentSPtr.Get())
		);
	}
}

void USkill_Talent_NuQi::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (OnValueChanged)
	{
		OnValueChanged->UnBindCallback();
	}

	if (CharacterPtr)
	{
		CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(AbilityActivatedCallbacksHandle);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Talent_NuQi::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Talent_NuQi::Tick(float DeltaTime)
{
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
		}
		else if (bIsInWeak)
		{
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
		if (TalentSPtr)
		{
			TalentSPtr->AddCurrentValue(AttackIncrement);
			if (TalentSPtr->GetCurrentValue() >= 100)
			{
				StartFuryState();
			}
		}
	}
}

void USkill_Talent_NuQi::SubNuQi(float Inveral)
{
	auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
	auto TalentPtr = dynamic_cast<FCurrentTalentType*>(CharacterAttributes.TalentSPtr.Get());
	if (TalentPtr)
	{
		if (bIsInWeak)
		{
			const auto Value = TalentPtr->GetMaxValue() / FMath::FloorToFloat(WeakDuration);
			TalentPtr->AddCurrentValue(-Value);
		}
		else
		{
			DecrementTime_Accumulate += Inveral;
			if (DecrementTime_Accumulate > DecrementTime)
			{
				TalentPtr->AddCurrentValue(-Decrement);
			}
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
		TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

		ModifyPropertyMap.Add(ECharacterPropertyType::AD, 50);

		CharacterPtr->GetBaseFeatureComponent()->SendEvent2Self(ModifyPropertyMap, SkillUnitPtr->GetUnitType());
	}

	auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
	if (EffectPtr)
	{
		EffectItemPtr = EffectPtr->AddEffectItem();
	}
}

void USkill_Talent_NuQi::StopFuryState()
{
	bIsInFury = false;

	if (CharacterPtr)
	{
		CharacterPtr->GetBaseFeatureComponent()->SendEvent2Self(GetAllData(), SkillUnitPtr->GetUnitType());
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

void USkill_Talent_NuQi::OnSendDamage(UGameplayAbility* GAPtr)
{
	if (CharacterPtr)
	{
		if (
			GAPtr &&
			(GAPtr->GetCurrentAbilitySpecHandle() == CharacterPtr->GetBaseFeatureComponent()->SendEventHandle)
			)
		{
			AddNuQi();
		}
	}
}
