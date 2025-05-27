#include "Skill_Passive_MultipleDamega.h"

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
#if UE_EDITOR || UE_SERVER
			if (
				(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
			)
			{
				auto PassiveSkillProxyPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillProxyPtr);
				if (!PassiveSkillProxyPtr)
				{
					return;
				}
				if (!PassiveSkillProxyPtr->CheckNotInCooldown())
				{
					return;
				}
				
				auto GameplayTagContainer = FGameplayTagContainer::EmptyContainer;
				GameplayTagContainer.AddTag(UGameplayTagsLibrary::GEData_Info);
				GameplayTagContainer.AddTag(SkillProxyPtr->GetProxyType());

				const auto GameplayEffectHandleAry = GetAbilitySystemComponentFromActorInfo()->GetActiveEffects(
					 FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(GameplayTagContainer)
					);
				if (!GameplayEffectHandleAry.IsEmpty())
				{
					return;
				}

				{
					FGameplayEffectSpecHandle SpecHandle =
						MakeOutgoingGameplayEffectSpec(ItemProxy_DescriptionPtr->GEClass, GetAbilityLevel());

					SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_Info);
					SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(
					                                               UGameplayTagsLibrary::GEData_Duration,
					                                               ItemProxy_DescriptionPtr->Duration.PerLevelValue[
						                                               0]
					                                              );
					SpecHandle.Data.Get()->SetStackCount(ItemProxy_DescriptionPtr->Count);

					EffectDurationHandle = ApplyGameplayEffectSpecToOwner(
					                                                      GetCurrentAbilitySpecHandle(),
					                                                      GetCurrentActorInfo(),
					                                                      GetCurrentActivationInfo(),
					                                                      SpecHandle
					                                                     );

					auto OnActiveGameplayEffectStackChangePtr = GetAbilitySystemComponentFromActorInfo()->
						OnGameplayEffectRemoved_InfoDelegate(EffectDurationHandle);
					if (OnActiveGameplayEffectStackChangePtr)
					{
						if (OnActiveGameplayEffectStackChangePtr->IsBound())
						{
						}
						else
						{
							OnActiveGameplayEffectStackChangePtr->AddUObject(
							                                                 this,
							                                                 &ThisClass::OnGameplayEffectRemoved
							                                                );
						}
					}
				}
				OutputDataModifySPtr = MakeShared<
					IOutputData_MultipleDamega_ModifyInterface>(
					                                            101,
					                                            ItemProxy_DescriptionPtr->Count,
					                                            ItemProxy_DescriptionPtr->Multiple
					                                           );
				HandleSPtr = OutputDataModifySPtr->CallbackContainerHelper.AddOnValueChanged(
					 std::bind(&ThisClass::OnReaminCountChanged, this, std::placeholders::_2)
					);
				CharacterPtr->GetCharacterAbilitySystemComponent()->AddOutputModify(OutputDataModifySPtr);
			}
#endif
		}
	}
}

void USkill_Passive_MultipleDamega::OnGameplayEffectRemoved(
	const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo
	)
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		{
			UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
			if (CooldownGE)
			{
				FGameplayEffectSpecHandle SpecHandle =
					MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_CD);
				SpecHandle.Data.Get()->AddDynamicAssetTag(SkillProxyPtr->GetProxyType());
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
															   UGameplayTagsLibrary::GEData_Duration,
															   ItemProxy_DescriptionPtr->CD.
															   PerLevelValue[0]
															  );

				CDHandle = ApplyGameplayEffectSpecToOwner(
														  GetCurrentAbilitySpecHandle(),
														  GetCurrentActorInfo(),
														  GetCurrentActivationInfo(),
														  SpecHandle
														 );
			}
		}
	}
#endif
}

void USkill_Passive_MultipleDamega::OnReaminCountChanged(
	int32 Count
	)
{
#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		auto GameplayTagContainer = FGameplayTagContainer::EmptyContainer;
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::GEData_Info);
		GameplayTagContainer.AddTag(SkillProxyPtr->GetProxyType());

		const auto GameplayEffectHandleAry = GetAbilitySystemComponentFromActorInfo()->GetActiveEffects(
			 FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(GameplayTagContainer)
			);
		if (GameplayEffectHandleAry.IsEmpty())
		{
		}
		else
		{
			for (const auto GEIter : GameplayEffectHandleAry)
			{
				auto GameplayEffectPtr =
					GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(
						 GEIter,
						 1
						);
			}
		}
	}
#endif
}
