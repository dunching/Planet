
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
		auto GameplayAbilityTargetDataPtr = CacheSPtr;

		if ((GameplayAbilityTargetDataPtr->PerformActionInterval < 0.f) && !GameplayAbilityTargetDataPtr->bOnluReFreshTime)
		{
			OnInterval(TaskPtr, GameplayAbilityTargetDataPtr->PerformActionInterval, GameplayAbilityTargetDataPtr->PerformActionInterval);
		}

		TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
		TaskPtr->UpdateDuration();

		StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataPtr->Duration;
		StateDisplayInfoSPtr->Num = GameplayAbilityTargetDataAry.Num();
		StateDisplayInfoSPtr->DataChanged();
	}
}

void UCS_PeriodicPropertyModify::SetCache(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataPtr)
{
	CacheSPtr = GameplayAbilityTargetDataPtr;
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
			TaskPtr->OnFinished.BindLambda(std::bind(&ThisClass::OnTaskFinished, this, std::placeholders::_1));
		}
		else
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
			TaskPtr->OnFinished.BindLambda(std::bind(&ThisClass::OnTaskFinished_Continue, this, std::placeholders::_1));
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
			// 数值修改 不会被移除


			// 直接修改的属性，会移除
			{
				auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

				GameplayAbilityTargetDataPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
					GameplayAbilityTargetDataPtr->ModifyPropertyMap, GameplayAbilityTargetDataPtr->Tag
				);
			}
		}
	}
}

void UCS_PeriodicPropertyModify::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Duration)
{
	if (CurrentInterval > Duration)
	{
		if (GameplayAbilityTargetDataAry.IsValidIndex(0))
		{
		}
	}
	else
	{
		StateDisplayInfoSPtr->TotalTime = CurrentInterval;
		StateDisplayInfoSPtr->DataChanged();
	}
}

bool UCS_PeriodicPropertyModify::OnTaskFinished_Continue(UAbilityTask_TimerHelper* InTaskPtr)
{
	if (GameplayAbilityTargetDataAry.IsValidIndex(0))
	{
		auto GameplayAbilityTargetDataPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

		GameplayAbilityTargetDataAry.RemoveAt(GameplayAbilityTargetDataAry.Num() - 1);


		// 移除周期内的属性
		{
			for (auto& Iter : GameplayAbilityTargetDataPtr->ModifyPropertyMap)
			{
				Iter.Value.SetCurrentValue(-Iter.Value.GetCurrentValue());
			}

			GameplayAbilityTargetDataPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
				GameplayAbilityTargetDataPtr->ModifyPropertyMap, GameplayAbilityTargetDataPtr->Tag
			);
		}

		if (GameplayAbilityTargetDataAry.IsEmpty())
		{
		}
		else
		{
			TaskPtr->UpdateDuration();
			TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->LosePropertyNumInterval);

			StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataPtr->LosePropertyNumInterval;
			StateDisplayInfoSPtr->Num = GameplayAbilityTargetDataAry.Num();
			StateDisplayInfoSPtr->DataChanged();
		}

		return false;
	}
	else
	{
		K2_CancelAbility();
		return true;
	}
}

bool UCS_PeriodicPropertyModify::OnTaskFinished(UAbilityTask_TimerHelper* InTaskPtr)
{
	K2_CancelAbility();
	return true;
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
	bool bInOnluReFreshTime,
	float InDuration,
	float InLosePropertyNumInterval
) :
	Super(Tag),
	Duration(InDuration),
	LosePropertyNumInterval(InLosePropertyNumInterval),
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
