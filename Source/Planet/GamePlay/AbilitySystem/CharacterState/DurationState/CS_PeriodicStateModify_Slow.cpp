
#include "CS_PeriodicStateModify_Slow.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureGAComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"

FGameplayAbilityTargetData_StateModify_Slow::FGameplayAbilityTargetData_StateModify_Slow(
	int32 InSpeedOffset
):
	Super(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Ice, 5.f),
	SpeedOffset(InSpeedOffset)
{
}

FGameplayAbilityTargetData_StateModify_Slow::FGameplayAbilityTargetData_StateModify_Slow()
{

}

FGameplayAbilityTargetData_StateModify_Slow* FGameplayAbilityTargetData_StateModify_Slow::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_StateModify_Slow;

	*ResultPtr = *this;

	return ResultPtr;
}

UCS_PeriodicStateModify_Slow::UCS_PeriodicStateModify_Slow():
	Super()
{

}

void UCS_PeriodicStateModify_Slow::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

}

void UCS_PeriodicStateModify_Slow::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Slow::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Slow::UpdateDuration()
{
	PerformAction();

	Super::UpdateDuration();

	if (TaskPtr)
	{
		TSharedPtr<FGameplayAbilityTargetData_StateModify_Slow> CurrentGameplayAbilityTargetDataSPtr(
			GameplayAbilityTargetDataSPtr,
			dynamic_cast<FGameplayAbilityTargetData_StateModify_Slow*>(GameplayAbilityTargetDataSPtr.Get())
		);

		if (MoveSpeedOffsetMap.Contains(CurrentGameplayAbilityTargetDataSPtr->SpeedOffset))
		{
			if (
				MoveSpeedOffsetMap[CurrentGameplayAbilityTargetDataSPtr->SpeedOffset].RemainTime >
				CurrentGameplayAbilityTargetDataSPtr->Duration
				)
			{
				return;
			}
		}

		FMyStruct MyStruct;

		MyStruct.RemainTime = CurrentGameplayAbilityTargetDataSPtr->Duration;
		MyStruct.SPtr = CurrentGameplayAbilityTargetDataSPtr;

		MoveSpeedOffsetMap.Add(
			CurrentGameplayAbilityTargetDataSPtr->SpeedOffset,
			MyStruct
		);

		auto MaxDuration = GameplayAbilityTargetDataSPtr->Duration;
		for (const auto Iter : MoveSpeedOffsetMap)
		{
			if (Iter.Value.RemainTime > MaxDuration)
			{
				MaxDuration = Iter.Value.RemainTime;
			}
		}

		TaskPtr->TickDelegate.BindUObject(this, &ThisClass::OnTaskTick);
		TaskPtr->SetDuration(MaxDuration);
		TaskPtr->UpdateDuration();

		StateDisplayInfoSPtr->Duration = MaxDuration;
		StateDisplayInfoSPtr->DataChanged();
	}
}

void UCS_PeriodicStateModify_Slow::PerformAction()
{
	Super::PerformAction();
}

void UCS_PeriodicStateModify_Slow::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	Super::OnDuration(InTaskPtr, CurrentInterval, Interval);
}

void UCS_PeriodicStateModify_Slow::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
	for (auto & Iter : MoveSpeedOffsetMap)
	{
		Iter.Value.RemainTime -= DeltaTime;
	}
	const auto Temp = MoveSpeedOffsetMap;
	for (auto Iter : Temp)
	{
		// 移除不生效的
		if (Iter.Value.RemainTime < 0.f)
		{
			MoveSpeedOffsetMap.Remove(Iter.Key);

			auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

			GAEventDataPtr->TriggerCharacterPtr = Iter.Value.SPtr->TriggerCharacterPtr;

			FGAEventData GAEventData(Iter.Value.SPtr->TargetCharacterPtr, Iter.Value.SPtr->TriggerCharacterPtr);

			GAEventData.DataSource = Iter.Value.SPtr->Tag;
			GAEventData.DataModify = { {ECharacterPropertyType::MoveSpeed,0 } };
			GAEventData.bIsOverlapData = true;

			GAEventDataPtr->DataAry.Add(GAEventData);

			CharacterPtr->GetInteractiveBaseGAComponent()->SendEventImp(GAEventDataPtr);
		}
	}

	for (const auto Iter : MoveSpeedOffsetMap)
	{
		auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

		GAEventDataPtr->TriggerCharacterPtr = Iter.Value.SPtr->TriggerCharacterPtr;

		FGAEventData GAEventData(Iter.Value.SPtr->TargetCharacterPtr, Iter.Value.SPtr->TriggerCharacterPtr);

		GAEventData.DataSource = Iter.Value.SPtr->Tag;
		GAEventData.DataModify = { {ECharacterPropertyType::MoveSpeed,Iter.Key } };

		GAEventDataPtr->DataAry.Add(GAEventData);

		CharacterPtr->GetInteractiveBaseGAComponent()->SendEventImp(GAEventDataPtr);
		break;
	}
}
