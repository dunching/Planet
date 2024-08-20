
#include "CS_PeriodicPropertyModify.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "InteractiveBaseGAComponent.h"
#include "SceneUnitTable.h"

UCS_PeriodicPropertyModify::UCS_PeriodicPropertyModify():
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UCS_PeriodicPropertyModify::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_PropertyModify*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			SetCache(TSharedPtr<FGameplayAbilityTargetData_PropertyModify>(GameplayAbilityTargetDataPtr->Clone()));
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicPropertyModify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicPropertyModify::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicPropertyModify::UpdateDuration()
{
	if (TaskPtr)
	{
		auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];
		TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
		TaskPtr->UpdateDuration();

		StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataPtr->Duration;
		StateDisplayInfoSPtr->Num = GameplayAbilityTargetDataAry.Num();
		StateDisplayInfoSPtr->DataChanged();
	}
}

void UCS_PeriodicPropertyModify::SetCache(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataPtr)
{
	if (GameplayAbilityTargetDataPtr->bOnluReFreshTime)
	{
	}
	else
	{
		GameplayAbilityTargetDataAry.Add(GameplayAbilityTargetDataPtr);
	}
}

void UCS_PeriodicPropertyModify::InitialStateDisplayInfo()
{
	Super::InitialStateDisplayInfo();

	if (GameplayAbilityTargetDataAry.IsValidIndex(0))
	{
		StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1]->Duration;
		StateDisplayInfoSPtr->TotalTime = 0.f;
	}
}

void UCS_PeriodicPropertyModify::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UCS_PeriodicPropertyModify::ExcuteTasks()
{
	if (GameplayAbilityTargetDataAry.IsValidIndex(0))
	{
		auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];
		TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

		if (GameplayAbilityTargetDataPtr->PerformActionInterval < 0.f)
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration);

			OnInterval(TaskPtr, GameplayAbilityTargetDataPtr->PerformActionInterval, GameplayAbilityTargetDataPtr->PerformActionInterval);
		}
		else
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
			TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);

			OnInterval(TaskPtr, GameplayAbilityTargetDataPtr->PerformActionInterval, GameplayAbilityTargetDataPtr->PerformActionInterval);
		}
		
		if (GameplayAbilityTargetDataPtr->LosePropertyNumInterval < 0.f)
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
			TaskPtr->OnFinished.BindLambda([this](auto)->bool {
				K2_CancelAbility();
				return true;
				});
		}
		else
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
			TaskPtr->OnFinished.BindLambda([this](auto)->bool {
				if (GameplayAbilityTargetDataAry.IsValidIndex(0))
				{
					auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

					TaskPtr->UpdateDuration();
					TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->LosePropertyNumInterval);

					GameplayAbilityTargetDataAry.RemoveAt(GameplayAbilityTargetDataAry.Num() - 1);

					StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataPtr->LosePropertyNumInterval;
					StateDisplayInfoSPtr->Num = GameplayAbilityTargetDataAry.Num();
					StateDisplayInfoSPtr->DataChanged();

					return false;
				}
				else
				{
					K2_CancelAbility();
					return true;
				}
				});
		}

		TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
		TaskPtr->ReadyForActivation();
	}
	else
	{
		K2_CancelAbility();
	}
}

void UCS_PeriodicPropertyModify::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
		if (GameplayAbilityTargetDataAry.IsValidIndex(0))
		{
			auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

		//	CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(GameplayAbilityTargetDataPtr->ModifyPropertyMap);
		}
	}
}

void UCS_PeriodicPropertyModify::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Duration)
{
	if (CurrentInterval > Duration)
	{
		if (GameplayAbilityTargetDataAry.IsValidIndex(0))
		{
			auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

			TPair<ACharacterBase*, TMap<ECharacterPropertyType, FBaseProperty>>ModifyPropertyMap{ CharacterPtr, GameplayAbilityTargetDataPtr->ModifyPropertyMap };
			CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Other({ ModifyPropertyMap }, GameplayAbilityTargetDataPtr->DataSource);
		}
	}
	else
	{
		StateDisplayInfoSPtr->TotalTime = CurrentInterval;
		StateDisplayInfoSPtr->DataChanged();
	}
}

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify(UConsumableUnit* RightVal) :
	Super(RightVal->GetUnitType())
{
	Duration = RightVal->GetTableRowUnit_Consumable()->Duration;
	PerformActionInterval = RightVal->GetTableRowUnit_Consumable()->PerformActionInterval;
	ModifyPropertyMap = RightVal->GetTableRowUnit_Consumable()->ModifyPropertyMap;
	DefaultIcon = RightVal->GetIcon();
	LosePropertyNumInterval = -1.f;
}

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify()
{

}

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify(
	const FGameplayTag& Tag,
	TSoftObjectPtr<UTexture2D>Icon,
	float InDuration,
	float InPerformActionInterval,
	float InLosePropertyNumInterval,
	const TMap<ECharacterPropertyType, FBaseProperty>& InModifyPropertyMap
):
	Super(Tag),
	Duration(InDuration),
	PerformActionInterval(InPerformActionInterval),
	LosePropertyNumInterval(InLosePropertyNumInterval),
	ModifyPropertyMap(InModifyPropertyMap)
{
	DefaultIcon = Icon;
}

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify(
	const FGameplayTag& Tag,
	bool bInOnluReFreshTime
) :
	Super(Tag),
	bOnluReFreshTime(bInOnluReFreshTime)
{
}

FGameplayAbilityTargetData_PropertyModify* FGameplayAbilityTargetData_PropertyModify::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_PropertyModify;

	*ResultPtr = *this;

	return ResultPtr;
}
