#include "Skill_Passive_MultipleDamega.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "CharacterAbilitySystemComponent.h"
#include "StateProcessorComponent.h"

#include "GameplayTagsLibrary.h"
#include "GE_Common.h"
#include "OnEffectedTawrgetCallback.h"

void UItemDecription_Skill_PassiveSkill_MultipleDamega::SetUIStyle()
{
}

void USkill_Passive_MultipleDamega::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle =
			CharacterPtr->GetCharacterAbilitySystemComponent()->MakedDamageDelegate.AddCallback(
				 std::bind(&ThisClass::MakedDamageDelegate, this, std::placeholders::_1)
				);
	}

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
		                                                            DynamicCastSharedPtr<FPassiveSkillProxy>(
			                                                             SkillProxyPtr
			                                                            )->GetTableRowProxy_PassiveSkillExtendInfo()
		                                                           );
	}
}

void USkill_Passive_MultipleDamega::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	if (AbilityActivatedCallbacksHandle)
	{
		AbilityActivatedCallbacksHandle->UnBindCallback();
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Passive_MultipleDamega::MakedDamageDelegate(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
	)
{
	if (CharacterPtr)
	{
		if (ReceivedEventModifyDataCallback.AllAssetTags.HasTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
		{
			auto AbilityTask_TimerHelperPtr = UAbilityTask_TimerHelper::DelayTask(this);
			AbilityTask_TimerHelperPtr->SetDuration(ItemProxy_DescriptionPtr->Duration.PerLevelValue[0]);
			AbilityTask_TimerHelperPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
			AbilityTask_TimerHelperPtr->OnFinished.BindUObject(this, &ThisClass::OnTimerFinished);
			AbilityTask_TimerHelperPtr->ReadyForActivation();

#if UE_EDITOR || UE_SERVER
			if (
				(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
			)
			{
				OutputDataModifySPtr = MakeShared<
					IOutputData_MultipleDamega_ModifyInterface>(
					                                            101,
					                                            ItemProxy_DescriptionPtr->Count,
					                                            ItemProxy_DescriptionPtr->Multiple
					                                           );
				CharacterPtr->GetCharacterAbilitySystemComponent()->AddOutputModify(OutputDataModifySPtr);
			}
#endif

			bIsRead = false;
		}
	}
}

void USkill_Passive_MultipleDamega::DurationDelegate(
	UAbilityTask_TimerHelper* TaskPtr,
	float CurrentInterval,
	float Duration
	)
{
}

bool USkill_Passive_MultipleDamega::OnTimerFinished(
	UAbilityTask_TimerHelper* TaskPtr
	)
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		CharacterPtr->GetCharacterAbilitySystemComponent()->RemoveOutputModify(OutputDataModifySPtr);
	}
#endif

	bIsRead = true;

	return true;
}
