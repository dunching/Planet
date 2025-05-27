#include "Skill_Element_Metal.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "AbilityTask_TimerHelper.h"
#include "AssetRefMap.h"
#include "AS_Character.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"

void USkill_Element_Metal::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
	)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	ItemProxy_DescriptionPtr = ItemProxy_Description_Ref.LoadSynchronous();

#if UE_EDITOR || UE_SERVER
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		if (CharacterPtr)
		{
			auto CharacterAttributeSetPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			auto AbilitySystemComponentPtr = GetAbilitySystemComponentFromActorInfo();
			AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
			                                                                   CharacterAttributeSetPtr->
			                                                                   GetMetalLevelAttribute()
			                                                                  ).AddUObject(
				 this,
				 &ThisClass::OnValueChanged
				);
		}
	}
#endif
}

void USkill_Element_Metal::OnValueChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	if (!ItemProxy_DescriptionPtr)
	{
		return;
	}

	const auto Value = static_cast<int32>(CurrentValue.NewValue);

	switch (Value)
	{
	case 7:
	case 8:
	case 9:
		{
		}
	case 4:
	case 5:
	case 6:
		{
		}
	case 1:
	case 2:
	case 3:
		{
			auto GASPtr = GetAbilitySystemComponentFromActorInfo();
			{
				auto SpecHandle = GASPtr->MakeOutgoingSpec(
														   UAssetRefMap::GetInstance()->OnceGEClass,
														   1,
														   GASPtr->MakeEffectContext()
														  );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalHitRate);

				if (!ItemProxy_DescriptionPtr->PerLevel_CriticalHitRate.PerLevelValue.IsValidIndex(Value - 1))
				{
					return;
				}
				
				const auto LevelValue = ItemProxy_DescriptionPtr->PerLevel_CriticalHitRate.PerLevelValue[Value - 1];
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
															   UGameplayTagsLibrary::DataSource_Elemental_Metal,
															   Value * LevelValue
															  );
				GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
			{
				auto SpecHandle = GASPtr->MakeOutgoingSpec(
														   UAssetRefMap::GetInstance()->OnceGEClass,
														   1,
														   GASPtr->MakeEffectContext()
														  );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_Temporary);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalDamage);

				if (!ItemProxy_DescriptionPtr->PerLevel_CriticalDamage.PerLevelValue.IsValidIndex(Value - 1))
				{
					return;
				}
				
				const auto LevelValue = ItemProxy_DescriptionPtr->PerLevel_CriticalDamage.PerLevelValue[Value - 1];
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
															   UGameplayTagsLibrary::DataSource_Elemental_Metal,
															   Value * LevelValue
															  );
				GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		break;
	default:
		{
			auto GASPtr = GetAbilitySystemComponentFromActorInfo();
			{
				auto SpecHandle = GASPtr->MakeOutgoingSpec(
														   UAssetRefMap::GetInstance()->OnceGEClass,
														   1,
														   GASPtr->MakeEffectContext()
														  );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalHitRate);

				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
															   UGameplayTagsLibrary::DataSource_Elemental_Metal,
															   0
															  );
				GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
			{
				auto SpecHandle = GASPtr->MakeOutgoingSpec(
														   UAssetRefMap::GetInstance()->OnceGEClass,
														   1,
														   GASPtr->MakeEffectContext()
														  );

				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyType_RemoveTemporary);
				SpecHandle.Data.Get()->AddDynamicAssetTag(UGameplayTagsLibrary::GEData_ModifyItem_CriticalDamage);

				SpecHandle.Data.Get()->SetSetByCallerMagnitude(
															   UGameplayTagsLibrary::DataSource_Elemental_Metal,
															   0
															  );
				GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		};
	}
}
