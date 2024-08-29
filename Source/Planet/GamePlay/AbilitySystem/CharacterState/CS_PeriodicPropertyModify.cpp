
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
		auto GameplayAbilityTargetDataSPtr = dynamic_cast<const FGameplayAbilityTargetData_PropertyModify*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataSPtr)
		{
			SetCache(TSharedPtr<FGameplayAbilityTargetData_PropertyModify>(GameplayAbilityTargetDataSPtr->Clone()));
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
		auto GameplayAbilityTargetDataSPtr = CacheSPtr;

		if ((GameplayAbilityTargetDataSPtr->PerformActionInterval < 0.f) && !GameplayAbilityTargetDataSPtr->bOnluReFreshTime)
		{
			OnInterval(TaskPtr, GameplayAbilityTargetDataSPtr->PerformActionInterval, GameplayAbilityTargetDataSPtr->PerformActionInterval);
		}

		TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration, GameplayAbilityTargetDataSPtr->PerformActionInterval);
		TaskPtr->UpdateDuration();

		StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
		StateDisplayInfoSPtr->Num = GameplayAbilityTargetDataAry.Num();
		StateDisplayInfoSPtr->DataChanged();
	}
}

void UCS_PeriodicPropertyModify::SetCache(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataSPtr)
{
	CacheSPtr = GameplayAbilityTargetDataSPtr;
	if (GameplayAbilityTargetDataSPtr->bOnluReFreshTime)
	{
	}
	else
	{
		GameplayAbilityTargetDataAry.Add(GameplayAbilityTargetDataSPtr);
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
		auto GameplayAbilityTargetDataSPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];
		TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);

		if (GameplayAbilityTargetDataSPtr->PerformActionInterval < 0.f)
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);

			OnInterval(TaskPtr, GameplayAbilityTargetDataSPtr->PerformActionInterval, GameplayAbilityTargetDataSPtr->PerformActionInterval);
		}
		else
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration, GameplayAbilityTargetDataSPtr->PerformActionInterval);
			TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);

			OnInterval(TaskPtr, GameplayAbilityTargetDataSPtr->PerformActionInterval, GameplayAbilityTargetDataSPtr->PerformActionInterval);
		}
		
		if (GameplayAbilityTargetDataSPtr->LosePropertyNumInterval < 0.f)
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration, GameplayAbilityTargetDataSPtr->PerformActionInterval);
			TaskPtr->OnFinished.BindLambda(std::bind(&ThisClass::OnTaskFinished, this, std::placeholders::_1));
		}
		else
		{
			TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration, GameplayAbilityTargetDataSPtr->PerformActionInterval);
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
				auto GameplayAbilityTargetDataSPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

				GameplayAbilityTargetDataSPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
					GameplayAbilityTargetDataSPtr->ModifyPropertyMap, GameplayAbilityTargetDataSPtr->Tag
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
		auto GameplayAbilityTargetDataSPtr = GameplayAbilityTargetDataAry[GameplayAbilityTargetDataAry.Num() - 1];

		GameplayAbilityTargetDataAry.RemoveAt(GameplayAbilityTargetDataAry.Num() - 1);


		// 移除周期内的属性(按层数)
		{
			for (auto& Iter : GameplayAbilityTargetDataSPtr->ModifyPropertyMap)
			{
				Iter.Value.SetCurrentValue(-Iter.Value.GetCurrentValue());
			}

			GameplayAbilityTargetDataSPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
				GameplayAbilityTargetDataSPtr->ModifyPropertyMap, GameplayAbilityTargetDataSPtr->Tag
			);
		}

		if (GameplayAbilityTargetDataAry.IsEmpty())
		{
			// 
			GameplayAbilityTargetDataSPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->ClearData2Self(
				GameplayAbilityTargetDataSPtr->ModifyPropertyMap, GameplayAbilityTargetDataSPtr->Tag
			);
		}
		else
		{
			TaskPtr->UpdateDuration();
			TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->LosePropertyNumInterval);

			StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->LosePropertyNumInterval;
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

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify(const FGameplayTag& Tag,
	bool bClear)
{
	bOnluReFreshTime = true;
	Duration = 0.f;
}

FGameplayAbilityTargetData_PropertyModify* FGameplayAbilityTargetData_PropertyModify::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_PropertyModify;

	*ResultPtr = *this;

	return ResultPtr;
}
