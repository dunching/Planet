
#include "Skill_Passive_ZMJZ.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "BaseFeatureComponent.h"
#include "StateProcessorComponent.h"
#include "CS_PeriodicPropertyModify.h"
#include "GAEvent_Helper.h"
#include "CharacterStateInfo.h"

void USkill_Passive_ZMJZ::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle =
			CharacterPtr->GetBaseFeatureComponent()->MakedDamageDelegate.AddCallback(std::bind(&ThisClass::OnSendAttack, this, std::placeholders::_2));
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
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);
	CharacterStateInfoSPtr = nullptr;

	const auto UnitType = SkillUnitPtr->GetUnitType();
	ModifyCharacterData(UnitType, true);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Passive_ZMJZ::PerformAction()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterPtr)
		{
			const auto UnitType = SkillUnitPtr->GetUnitType();
			CharacterStateInfoSPtr = CharacterPtr->GetStateProcessorComponent()->GetCharacterState(UnitType);
			if (CharacterStateInfoSPtr)
			{
				if (CharacterStateInfoSPtr->Num < MaxCount)
				{
					CharacterStateInfoSPtr->Num++;
					CharacterStateInfoSPtr->DataChanged();
					CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);

					ModifyCharacterData(UnitType, SpeedOffset);
				}
				else
				{
				}

				if (TimerTaskPtr)
				{
					TimerTaskPtr->SetDuration(DecreamTime, 0.1f);
					TimerTaskPtr->UpdateDuration();
				}
			}
			else
			{
				CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
				CharacterStateInfoSPtr->Num = 1;
				CharacterStateInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
				CharacterStateInfoSPtr->Duration = DecreamTime;
				CharacterStateInfoSPtr->DefaultIcon = BuffIcon;
				CharacterStateInfoSPtr->DataChanged();

				CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

				ModifyCharacterData(UnitType, SpeedOffset);

				{
					TimerTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
					TimerTaskPtr->SetDuration(DecreamTime, 0.1f);
					TimerTaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
					TimerTaskPtr->OnFinished.BindUObject(this, &ThisClass::OnTimerTaskFinished);
					TimerTaskPtr->ReadyForActivation();
				}
			}
		}
	}
#endif
}

void USkill_Passive_ZMJZ::OnSendAttack(const FGAEventData& GAEventData)
{
	if (CharacterPtr)
	{
		if ((GAEventData.GetIsHited()) && GAEventData.bIsWeaponAttack)
		{
			PerformAction();
		}
	}
}

void USkill_Passive_ZMJZ::DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Duration)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = Duration - CurrentInterval;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
		}
	}
#endif
}

bool USkill_Passive_ZMJZ::OnTimerTaskFinished(UAbilityTask_TimerHelper* TaskPtr)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			const auto UnitType = SkillUnitPtr->GetUnitType();

			CharacterStateInfoSPtr->Num--;
			if (CharacterStateInfoSPtr->Num <= 0)
			{
				ModifyCharacterData(UnitType, 0, true);

				CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);
				CharacterStateInfoSPtr = nullptr;
				return true;
			}
			else
			{
				ModifyCharacterData(UnitType, -SpeedOffset);

				CharacterStateInfoSPtr->Duration = SecondaryDecreamTime;
				CharacterStateInfoSPtr->RefreshTime();
				CharacterStateInfoSPtr->DataChanged();
				CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);

				TimerTaskPtr->SetDuration(SecondaryDecreamTime, 0.1f);
				TimerTaskPtr->UpdateDuration();

				return false;
			}
		}
	}
#endif
	return true;
}

void USkill_Passive_ZMJZ::ModifyCharacterData(
	const FGameplayTag& DataSource,
	int32 Value, 
	bool bIsClear
)
{
	auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);
	GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;

	FGAEventData GAEventData(CharacterPtr, CharacterPtr);
	GAEventData.TriggerCharacterPtr = CharacterPtr;
	GAEventData.TargetCharacterPtr = CharacterPtr;

	TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

	ModifyPropertyMap.Add(ECharacterPropertyType::GAPerformSpeed, Value);

	GAEventData.DataSource = DataSource;
	if (bIsClear)
	{
		GAEventData.DataModify = GetAllData();
		GAEventData.bIsClearData = true;
	}
	else
	{
		GAEventData.DataModify = ModifyPropertyMap;
	}

	GameplayAbilityTargetDataPtr->DataAry.Add(GAEventData);

	auto ICPtr = CharacterPtr->GetBaseFeatureComponent();
	ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
}

