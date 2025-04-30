#include "Skill_Passive_XR.h"

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
					std::bind(&ThisClass::OnSendAttack, this, std::placeholders::_1)
				);
		}
	}
#endif
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

void USkill_Passive_XR::OnSendAttack(
	const FOnEffectedTawrgetCallback& ReceivedEventModifyDataCallback
)
{
	if (CharacterPtr)
	{
		if (ReceivedEventModifyDataCallback.AllAssetTags.HasTag(UGameplayTagsLibrary::Proxy_Weapon))
		{
			auto CharacterProxySPtr = CharacterPtr->GetCharacterProxy();
			auto HoldingItemsComponentPtr = CharacterPtr->GetGroupManagger()->GetHoldingItemsComponent();
			
			const auto ActiveSocket_1 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::ActiveSocket_1);
			auto SkillSPtr = HoldingItemsComponentPtr->FindProxy_Skill(ActiveSocket_1.GetAllocationedProxyID());
			
			const auto ActiveSocket_2 = CharacterProxySPtr->FindSocket(UGameplayTagsLibrary::ActiveSocket_2);
		}
	}
}
