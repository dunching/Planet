#include "GEEC_Common.h"

#include "AS_Character.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"
#include "BaseFeatureComponent.h"
#include "GE_Common.h"

void UGEEC_Base::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
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

	Instigator->GetBaseFeatureComponent()->OnReceivedEventModifyData(
		CustomMagnitudes,
		ExecutionParams,
		OutExecutionOutput
	);
}

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

	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_ModifyItem_HP, SourceSet->GetHP_Replay());
	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_ModifyItem_PP, SourceSet->GetPP_Replay());
	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_ModifyItem_Mana, SourceSet->GetMana_Replay());

	Instigator->GetBaseFeatureComponent()->OnReceivedEventModifyData(
		CustomMagnitudes,
		ExecutionParams,
		OutExecutionOutput
	);
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

	const auto MoveSpeedOffset = Cast<UGE_Running>(Spec.Def)->MoveSpeedOffset.GetValue();
	CustomMagnitudes.Add(UGameplayTagsLibrary::DataSource_Character, MoveSpeedOffset);

	Instigator->GetBaseFeatureComponent()->OnReceivedEventModifyData(CustomMagnitudes, ExecutionParams,
	                                                                 OutExecutionOutput);
}

void UGEEC_CancelRunning::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
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

	CustomMagnitudes.Add(UGameplayTagsLibrary::DataSource_Character, 0);

	Instigator->GetBaseFeatureComponent()->OnReceivedEventModifyData(CustomMagnitudes, ExecutionParams,
	                                                                 OutExecutionOutput);
}
