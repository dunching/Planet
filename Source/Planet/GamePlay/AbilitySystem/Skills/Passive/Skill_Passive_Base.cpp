#include "Skill_Passive_Base.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/RichTextBlock.h"

#include "SceneProxyTable.h"
#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttibutes.h"
#include "GenerateTypes.h"
#include "CharacterAttributesComponent.h"
#include "PropertyEntrys.h"
#include "TemplateHelper.h"
#include "PropertyEntryDescription.h"
#include "Kismet/KismetStringLibrary.h"

void UItemDecription_Skill_PassiveSkill::SetUIStyle()
{
	if (ProxySPtr)
	{
		{
			if (Title)
			{
				Title->SetText(FText::FromString(ProxySPtr->GetProxyName()));
			}
		}
		auto SkillProxySPtr = DynamicCastSharedPtr<FSkillProxyType>(ProxySPtr);
		if (!SkillProxySPtr)
		{
			return;
		}

		if (PropertyEntrysVerticalBox)
		{
			PropertyEntrysVerticalBox->ClearChildren();

			for (const auto& Iter : SkillProxySPtr->GeneratedPropertyEntryAry)
			{
				auto UIPtr = CreateWidget<UPropertyEntryDescription>(this, PropertyEntryDescriptionClass);
				if (UIPtr)
				{
					UIPtr->SetDta(Iter.second);
					PropertyEntrysVerticalBox->AddChild(UIPtr);
				}
			}
		}
	}
	else if (ProxyType.IsValid())
	{
		{
			if (Title)
			{
				Title->SetText(FText::FromString(ItemProxy_Description->ProxyName));
			}
		}
	}
	
	
	auto ItemProxy_DescriptionPtr = ItemProxy_Description.LoadSynchronous();
	if (ItemProxy_DescriptionPtr && !ItemProxy_DescriptionPtr->DecriptionText.IsEmpty())
	{
		FString Text = ItemProxy_DescriptionPtr->DecriptionText[0];
		for (const auto& Iter : ItemProxy_DescriptionPtr->Values)
		{
			if (Iter.Value.PerLevelValue.IsEmpty())
			{
				continue;
			}

			Text = Text.Replace(*Iter.Key, *UKismetStringLibrary::Conv_IntToString(Iter.Value.PerLevelValue[0]));
		}

		if (DescriptionText)
		{
			DescriptionText->SetText(FText::FromString(Text));
		}
	}
}

USkill_Passive_Base::USkill_Passive_Base() :
                                           Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Passive_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CallActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());

	if (CharacterPtr)
	{
		auto PassiveSkillProxyPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillProxyPtr);
	}
}

void USkill_Passive_Base::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	if (CharacterPtr)
	{
		auto PassiveSkillProxyPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillProxyPtr);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}
