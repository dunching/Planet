#include "GEEC_Common.h"

#include "AS_Character.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"
#include "BaseFeatureComponent.h"

void UGEEC_Reply::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	// Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	if (
		Instigator->GetAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::DeathingTag) ||
		Instigator->GetAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
	)
	{
		return;
	}
	else
	{
	}

	TMap<FGameplayTag, float> CustomMagnitudes;

	// 获得来源AttributeSet
	const auto SourceSet = Cast<UAS_Character>(
		ExecutionParams.GetSourceAbilitySystemComponent()->GetAttributeSet(UAS_Character::StaticClass()));

	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_HP, SourceSet->GetHP_Replay());

	Instigator->GetBaseFeatureComponent()->OnReceivedEventModifyData(CustomMagnitudes, ExecutionParams,
	                                                                 OutExecutionOutput);
}

void UGEEC_Running::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	if (
		Instigator->GetAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::DeathingTag) ||
		Instigator->GetAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
	)
	{
		return;
	}
	else
	{
	}

	TMap<FGameplayTag, float> CustomMagnitudes;

	// 获得来源AttributeSet
	const auto SourceSet = Cast<UAS_Character>(
		ExecutionParams.GetSourceAbilitySystemComponent()->GetAttributeSet(UAS_Character::StaticClass()));

	Instigator->GetBaseFeatureComponent()->OnReceivedEventModifyData(CustomMagnitudes, ExecutionParams,
																	 OutExecutionOutput);
}
