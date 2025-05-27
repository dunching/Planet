#include "Skill_Passive_XR.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateTypes.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "CharacterAbilitySystemComponent.h"
#include "StateProcessorComponent.h"

#include "GameplayTagsLibrary.h"
#include "GE_Common.h"
#include "GroupManagger.h"
#include "InventoryComponent.h"
#include "OnEffectedTawrgetCallback.h"

void UItemDecription_Skill_PassiveSkill_XR::SetUIStyle()
{
}

void USkill_Passive_XR::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
		if (CharacterPtr)
		{
			AbilityActivatedCallbacksHandle =
				CharacterPtr->GetCharacterAbilitySystemComponent()->MakedDamageDelegate.AddCallback(
					std::bind(&ThisClass::MakedDamageDelegate, this, std::placeholders::_1)
				);
		}
	}
#endif

	if (SkillProxyPtr)
	{
		ItemProxy_DescriptionPtr = Cast<FItemProxy_DescriptionType>(
			DynamicCastSharedPtr<FPassiveSkillProxy>(SkillProxyPtr)->GetTableRowProxy_PassiveSkillExtendInfo()
		);
	}
}

void USkill_Passive_XR::OnRemoveAbility(
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

void USkill_Passive_XR::MakedDamageDelegate(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
)
{
	if (CharacterPtr)
	{
		if (ReceivedEventModifyDataCallback.AllAssetTags.HasTag(UGameplayTagsLibrary::Proxy_Skill_Weapon))
		{
			auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();
			auto HoldingItemsComponentPtr = CharacterPtr->GetGroupManagger()->GetInventoryComponent();

			TArray<TSharedPtr<FActiveSkillProxy>> ActiveSkillProxyAry;
			const auto ActiveSocket_1 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::ActiveSocket_1);
			auto ActiveSkillProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(
					HoldingItemsComponentPtr->FindProxy_Skill(ActiveSocket_1.GetAllocationedProxyID())
				);
			if (ActiveSkillProxySPtr && !ActiveSkillProxySPtr->CheckNotInCooldown())
			{
				ActiveSkillProxyAry.Add(
				ActiveSkillProxySPtr
				);
			}

			const auto ActiveSocket_2 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::ActiveSocket_2);
			ActiveSkillProxySPtr = DynamicCastSharedPtr<FActiveSkillProxy>(
					HoldingItemsComponentPtr->FindProxy_Skill(ActiveSocket_2.GetAllocationedProxyID())
				);
			if (ActiveSkillProxySPtr && !ActiveSkillProxySPtr->CheckNotInCooldown())
			{
				ActiveSkillProxyAry.Add(
				ActiveSkillProxySPtr
				);
			}

			if (!ActiveSkillProxyAry.IsEmpty())
			{
				for (;;)
				{
					const auto Index = FMath::RandRange(0, ActiveSkillProxyAry.Num() - 1);
					if (ActiveSkillProxyAry[Index])
					{
						ActiveSkillProxyAry[Index]->AddCooldownConsumeTime(-ItemProxy_DescriptionPtr->CD.PerLevelValue[0]);
						break;
					}
				}
			}
		}
	}
}
