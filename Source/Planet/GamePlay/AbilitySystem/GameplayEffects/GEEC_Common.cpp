#include "GEEC_Common.h"

#include "AS_Character.h"
#include "CharacterBase.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "GE_Common.h"

void UGEEC_Base::ApplyModifyData(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput,
	const TMap<FGameplayTag, float>& SetByCallerTagMagnitudes,
	TObjectPtr<ACharacterBase> InstigatorPtr,
	TObjectPtr<ACharacterBase> TargetCharacterPtr
	) const
{
	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	TSet<FGameplayTag> NeedModifySet;

	auto CustomMagnitudes = InstigatorPtr->GetCharacterAbilitySystemComponent()->ModifyOutputData(
		 AllAssetTags,
		 NeedModifySet,
		 SetByCallerTagMagnitudes,
		 ExecutionParams,
		 OutExecutionOutput
		);

	CustomMagnitudes = TargetCharacterPtr->GetCharacterAbilitySystemComponent()->ModifyInputData(
		 AllAssetTags,
		 NeedModifySet,
		 CustomMagnitudes,
		 ExecutionParams,
		 OutExecutionOutput
		);

	TargetCharacterPtr->GetCharacterAbilitySystemComponent()->ApplyInputData(
	                                                                         AllAssetTags,
	                                                                         NeedModifySet,
	                                                                         CustomMagnitudes,
	                                                                         ExecutionParams,
	                                                                         OutExecutionOutput
	                                                                        );
}

void UGEEC_DataModify::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const
{
	// Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto TargetCharacterPtr = Cast<ACharacterBase>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	if (
		TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(
			 UGameplayTagsLibrary::State_Dying
			) ||
		TargetCharacterPtr->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(
			 UGameplayTagsLibrary::Respawning
			)
	)
	{
		return;
	}
	else
	{
	}

	TMap<FGameplayTag, float> CustomMagnitudes;
	CustomMagnitudes.Append(Spec.SetByCallerTagMagnitudes);

	ApplyModifyData(
	                ExecutionParams,
	                OutExecutionOutput,
	                CustomMagnitudes,
	                Instigator,
	                TargetCharacterPtr
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

	auto TargetCharacterPtr = Cast<ACharacterBase>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	if (
		Instigator->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::State_Dying) ||
		Instigator->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
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
	                                           ExecutionParams.GetSourceAbilitySystemComponent()->GetAttributeSet(
		                                            UAS_Character::StaticClass()
		                                           )
	                                          );

	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_ModifyItem_HP, SourceSet->GetHP_Replay());
	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_ModifyItem_Stamina, SourceSet->GetStamina_Replay());
	CustomMagnitudes.Add(UGameplayTagsLibrary::GEData_ModifyItem_Mana, SourceSet->GetMana_Replay());

	CustomMagnitudes.Append(Spec.SetByCallerTagMagnitudes);

	ApplyModifyData(
	                ExecutionParams,
	                OutExecutionOutput,
	                CustomMagnitudes,
	                Instigator,
	                TargetCharacterPtr
	               );
}

void UGEEC_Running::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const
{
	// Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto TargetCharacterPtr = Cast<ACharacterBase>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	if (
		Instigator->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::State_Dying) ||
		Instigator->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
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
	                                           ExecutionParams.GetSourceAbilitySystemComponent()->GetAttributeSet(
		                                            UAS_Character::StaticClass()
		                                           )
	                                          );

	const auto MoveSpeedOffset = Cast<UGE_Running>(Spec.Def)->MoveSpeedOffset.GetValue();
	CustomMagnitudes.Add(UGameplayTagsLibrary::DataSource_Character, MoveSpeedOffset);

	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	TSet<FGameplayTag> NeedModifySet;

	TargetCharacterPtr->GetCharacterAbilitySystemComponent()->ModifyOutputData(
	                                                                           AllAssetTags,
	                                                                           NeedModifySet,
	                                                                           CustomMagnitudes,
	                                                                           ExecutionParams,
	                                                                           OutExecutionOutput
	                                                                          );
}

void UGEEC_CancelRunning::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const
{
	// Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	// 获得对应GE对象
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	auto TargetCharacterPtr = Cast<ACharacterBase>(ExecutionParams.GetTargetAbilitySystemComponent()->GetOwnerActor());

	auto Instigator = Cast<ACharacterBase>(Context.GetInstigator());
	auto EffectCauser = Cast<ACharacterBase>(Context.GetEffectCauser());

	if (
		Instigator->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::State_Dying) ||
		Instigator->GetCharacterAbilitySystemComponent()->HasMatchingGameplayTag(UGameplayTagsLibrary::Respawning)
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
	                                           ExecutionParams.GetSourceAbilitySystemComponent()->GetAttributeSet(
		                                            UAS_Character::StaticClass()
		                                           )
	                                          );

	CustomMagnitudes.Add(UGameplayTagsLibrary::DataSource_Character, 0);

	FGameplayTagContainer AllAssetTags;
	Spec.GetAllAssetTags(AllAssetTags);

	TSet<FGameplayTag> NeedModifySet;

	TargetCharacterPtr->GetCharacterAbilitySystemComponent()->ModifyOutputData(
	                                                                           AllAssetTags,
	                                                                           NeedModifySet,
	                                                                           CustomMagnitudes,
	                                                                           ExecutionParams,
	                                                                           OutExecutionOutput
	                                                                          );
}
