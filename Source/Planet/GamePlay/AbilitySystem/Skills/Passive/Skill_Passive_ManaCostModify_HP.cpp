#include "Skill_Passive_ManaCostModify_HP.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "CharacterAbilitySystemComponent.h"

#include "ItemProxy_Skills.h"

void UItemDecription_Skill_PassiveSkill_ManaCostModify_HP::SetUIStyle()
{
}

void USkill_Passive_ManaCostModify_HP::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
		                                                            DynamicCastSharedPtr<FPassiveSkillProxy>(
			                                                             SkillProxyPtr
			                                                            )->GetTableRowProxy_PassiveSkillExtendInfo()
		                                                           );
	}

#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
		if (CharacterPtr)
		{
			ModifySPtr = MakeShared<IGostModify_ReplaceWithOther_Interface>(
			                                                                static_cast<int32>(
				                                                                EGostModifyOrder::kSkill_Passive_ManaCostModify_HP),
			                                                                ItemProxy_DescriptionPtr->
			                                                                InCostAttributeTag,
			                                                                ItemProxy_DescriptionPtr->InManaPercent,
			                                                                ItemProxy_DescriptionPtr->
			                                                                InNewResourcePercent
			                                                               );
			CharacterPtr->GetCharacterAbilitySystemComponent()->AddGostModify(ModifySPtr);
		}
	}
#endif
}

void USkill_Passive_ManaCostModify_HP::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		if (CharacterPtr)
		{
			CharacterPtr->GetCharacterAbilitySystemComponent()->RemoveGostModify(ModifySPtr);
			ModifySPtr = nullptr;
		}
	}
#endif

	Super::OnRemoveAbility(ActorInfo, Spec);
}
