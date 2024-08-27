
#include "Skill_Passive_ZMJZ.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "InteractiveBaseGAComponent.h"
#include "CS_PeriodicPropertyModify.h"
#include "CS_Base.h"

USkill_Passive_ZMJZ::USkill_Passive_ZMJZ() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Passive_ZMJZ::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle =
			CharacterPtr->GetInteractiveBaseGAComponent()->MakedDamage.AddCallback(std::bind(&ThisClass::OnSendAttack, this, std::placeholders::_2));
	}
}

void USkill_Passive_ZMJZ::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Passive_ZMJZ::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USkill_Passive_ZMJZ::OnRemoveAbility(
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

void USkill_Passive_ZMJZ::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Passive_ZMJZ::PerformAction()
{
	if (CharacterPtr)
	{
		auto CSPtr = CharacterPtr->GetInteractiveBaseGAComponent()->GetCharacterState(SkillUnitPtr->GetUnitType());
		if (CSPtr)
		{
			if (CSPtr->GetStateDisplayInfo().Pin()->Num < MaxCount)
			{
				TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

				ModifyPropertyMap.Add(ECharacterPropertyType::GAPerformSpeed, SpeedOffset);
				ModifyPropertyMap.Add(ECharacterPropertyType::MoveSpeed, SpeedOffset);

				auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(
					SkillUnitPtr->GetUnitType(),
					SkillUnitPtr->GetIcon(),
					DecreamTime,
					-1.f,
					SecondaryDecreamTime,
					ModifyPropertyMap
				);

				GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
				GameplayAbilityTargetDataPtr->TargetCharacterPtr = CharacterPtr;

				auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
				ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
			}
			else
			{
				auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(
					SkillUnitPtr->GetUnitType(),
					true,
					DecreamTime,
					SecondaryDecreamTime
				);

				GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
				GameplayAbilityTargetDataPtr->TargetCharacterPtr = CharacterPtr;

				auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
				ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
			}
		}
		else
		{
			TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

			ModifyPropertyMap.Add(ECharacterPropertyType::GAPerformSpeed, SpeedOffset);

			auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(
				SkillUnitPtr->GetUnitType(),
				SkillUnitPtr->GetIcon(),
				DecreamTime,
				-1.f,
				SecondaryDecreamTime,
				ModifyPropertyMap
			);

			GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetDataPtr->TargetCharacterPtr = CharacterPtr;

			auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
			ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
		}
	}
}

void USkill_Passive_ZMJZ::OnSendAttack(const FGAEventData& GAEventData)
{
	if (CharacterPtr)
	{
		if ((GAEventData.HitRate > 0) && GAEventData.bIsWeaponAttack)
		{
			PerformAction();
		}
	}
}

void USkill_Passive_ZMJZ::OnIntervalTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
}

