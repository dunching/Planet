
#include "CS_PeriodicPropertyModify.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureComponent.h"
#include "SceneUnitTable.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

UCS_PeriodicPropertyModify::UCS_PeriodicPropertyModify() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify(const TSharedPtr<FConsumableProxy>& RightVal) :
	Super(RightVal->GetUnitType())
{
	Duration = RightVal->GetTableRowUnit_Consumable()->Duration;
	PerformActionInterval = RightVal->GetTableRowUnit_Consumable()->PerformActionInterval;
	ModifyPropertyPerLayer = { RightVal->GetTableRowUnit_Consumable()->ModifyPropertyMap };
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
) :
	Super(Tag),
	Duration(InDuration),
	PerformActionInterval(InPerformActionInterval),
	LosePropertyNumInterval(InLosePropertyNumInterval),
	ModifyPropertyPerLayer{ InModifyPropertyMap }
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
	bIsOnlyReFreshTime(bInOnluReFreshTime)
{
}

FGameplayAbilityTargetData_PropertyModify::FGameplayAbilityTargetData_PropertyModify(
	const FGameplayTag& Tag,
	bool bClear
) :
	Super(Tag)
{
	bIsOnlyReFreshTime = true;
	Duration = 0.f;
	bIsClear = bClear;
}

FGameplayAbilityTargetData_PropertyModify* FGameplayAbilityTargetData_PropertyModify::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_PropertyModify;

	*ResultPtr = *this;

	return ResultPtr;
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

		if (DurationEffectMap.Contains(GameplayAbilityTargetDataSPtr->Tag))
		{
			if (GameplayAbilityTargetDataSPtr->bIsClear)
			{
				DurationEffectMap[GameplayAbilityTargetDataSPtr->Tag].CharacterStateInfoSPtr->SetFinished();
			}
			else if (GameplayAbilityTargetDataSPtr->bIsOnlyReFreshTime)
			{
				DurationEffectMap[GameplayAbilityTargetDataSPtr->Tag].CharacterStateInfoSPtr->RefreshTime();
			}
			else
			{
				DurationEffectMap[GameplayAbilityTargetDataSPtr->Tag].CharacterStateInfoSPtr->Duration =
					GameplayAbilityTargetDataSPtr->Duration;
				DurationEffectMap[GameplayAbilityTargetDataSPtr->Tag].CharacterStateInfoSPtr->RefreshTime();
				DurationEffectMap[GameplayAbilityTargetDataSPtr->Tag].SPtr = GameplayAbilityTargetDataSPtr;

				if (GameplayAbilityTargetDataSPtr->LosePropertyNumInterval < 0.f)
				{
					PerformPropertyModify(GameplayAbilityTargetDataSPtr);
				}
			}
		}
		else
		{
			FMyStruct MyStruct;

			MyStruct.CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
			MyStruct.CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
			MyStruct.CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
			MyStruct.CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
			MyStruct.CharacterStateInfoSPtr->DataChanged();
			MyStruct.SPtr = GameplayAbilityTargetDataSPtr;

			CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(MyStruct.CharacterStateInfoSPtr);

			DurationEffectMap.Add(
				GameplayAbilityTargetDataSPtr->Tag,
				MyStruct
			);

			PerformPropertyModify(GameplayAbilityTargetDataSPtr);
		}
	}
}

void UCS_PeriodicPropertyModify::SetCache(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& GameplayAbilityTargetDataSPtr)
{
	CacheSPtr = GameplayAbilityTargetDataSPtr;
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
	TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->TickDelegate.BindUObject(this, &ThisClass::OnTaskTick);
	TaskPtr->SetInfinite();
	TaskPtr->ReadyForActivation();

	UpdateDuration();
}

void UCS_PeriodicPropertyModify::OnTaskTick(
	UAbilityTask_TimerHelper*,
	float DeltaTime
)
{
	for (auto& Iter : DurationEffectMap)
	{
		Iter.Value.EffectTime += DeltaTime;
		Iter.Value.CharacterStateInfoSPtr->IncreaseCooldownTime(DeltaTime);
	}

	const auto Temp = DurationEffectMap;
	for (const auto& Iter : Temp)
	{
		// 到期的
		if (Iter.Value.CharacterStateInfoSPtr->GetIsFinished())
		{
			if ((Iter.Value.SPtr->LosePropertyNumInterval < 0.f) || Iter.Value.SPtr->bIsClear)
			{
				PerformPropertyModify(Iter.Value.SPtr);

				DurationEffectMap.Remove(Iter.Key);

				CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(Iter.Value.CharacterStateInfoSPtr);
			}
			else
			{
				if (Iter.Value.SPtr->ModifyPropertyPerLayer.IsEmpty())
				{
					PerformPropertyModify(Iter.Value.SPtr);

					DurationEffectMap.Remove(Iter.Key);
					CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(Iter.Value.CharacterStateInfoSPtr);
				}
				else
				{
					Iter.Value.SPtr->ModifyPropertyPerLayer.RemoveAt(Iter.Value.SPtr->ModifyPropertyPerLayer.Num() - 1);
					Iter.Value.CharacterStateInfoSPtr->Duration = Iter.Value.SPtr->LosePropertyNumInterval;
					Iter.Value.CharacterStateInfoSPtr->RefreshTime();

					PerformPropertyModify(Iter.Value.SPtr);
				}
			}
		}
		else
		{
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(Iter.Value.CharacterStateInfoSPtr);
		}
	}

	// 修改属性
	for (auto& Iter : DurationEffectMap)
	{
		if (Iter.Value.SPtr->PerformActionInterval < 0.f)
		{

		}
		else if (Iter.Value.EffectTime >= Iter.Value.SPtr->PerformActionInterval)
		{
			Iter.Value.EffectTime = 0.f;
			PerformPropertyModify(Iter.Value.SPtr);
		}
	}

	if (DurationEffectMap.IsEmpty())
	{
		K2_CancelAbility();
	}
}

void UCS_PeriodicPropertyModify::PerformPropertyModify(const TSharedPtr<FGameplayAbilityTargetData_PropertyModify>& SPtr)
{
	auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

	GAEventDataPtr->TriggerCharacterPtr = SPtr->TriggerCharacterPtr;

	FGAEventData GAEventData(SPtr->TargetCharacterPtr, SPtr->TriggerCharacterPtr);

	GAEventData.DataSource = SPtr->Tag;
	GAEventData.bIsOverlapData = !SPtr->bIsAdd;

	if (SPtr->ModifyPropertyPerLayer.IsEmpty())
	{
		// 
		GAEventData.bIsClearData = true;
	}
	else
	{
		GAEventData.DataModify = SPtr->ModifyPropertyPerLayer[SPtr->ModifyPropertyPerLayer.Num() - 1];
	}

	GAEventDataPtr->DataAry.Add(GAEventData);

	CharacterPtr->GetBaseFeatureComponent()->SendEventImp(GAEventDataPtr);
}
