
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
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_StateModify_Slow::FGameplayAbilityTargetData_StateModify_Slow(
	int32 InSpeedOffset
):
	Super(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Slow, 5.f),
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
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Slow);

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

	TaskPtr->SetInfinite();

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
	Super::UpdateDuration();

	PerformAction();
}

void UCS_PeriodicStateModify_Slow::PerformAction()
{
	Super::PerformAction();

	if (TaskPtr)
	{
		TSharedPtr<FGameplayAbilityTargetData_StateModify_Slow> CurrentGameplayAbilityTargetDataSPtr(
			GameplayAbilityTargetDataSPtr,
			dynamic_cast<FGameplayAbilityTargetData_StateModify_Slow*>(GameplayAbilityTargetDataSPtr.Get())
		);

		if (MoveSpeedOffsetMap.Contains(CurrentGameplayAbilityTargetDataSPtr->SpeedOffset))
		{
			if (
				MoveSpeedOffsetMap[CurrentGameplayAbilityTargetDataSPtr->SpeedOffset].CharacterStateInfoSPtr->GetRemainTime() >
				CurrentGameplayAbilityTargetDataSPtr->Duration
				)
			{
				return;
			}
		}

		FMyStruct MyStruct;

		MyStruct.CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		MyStruct.CharacterStateInfoSPtr->Tag = CurrentGameplayAbilityTargetDataSPtr->Tag;
		MyStruct.CharacterStateInfoSPtr->Duration = CurrentGameplayAbilityTargetDataSPtr->Duration;
		MyStruct.CharacterStateInfoSPtr->DefaultIcon = CurrentGameplayAbilityTargetDataSPtr->DefaultIcon;
		MyStruct.CharacterStateInfoSPtr->DataChanged();
		MyStruct.SPtr = CurrentGameplayAbilityTargetDataSPtr;

		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(MyStruct.CharacterStateInfoSPtr);

		MoveSpeedOffsetMap.Add(
			CurrentGameplayAbilityTargetDataSPtr->SpeedOffset,
			MyStruct
		);
	}
}

void UCS_PeriodicStateModify_Slow::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
	for (auto & Iter : MoveSpeedOffsetMap)
	{
		Iter.Value.CharacterStateInfoSPtr->TotalTime += DeltaTime;
	}
	const auto Temp = MoveSpeedOffsetMap;
	for (auto Iter : Temp)
	{
		// 移除不生效的
		if (Iter.Value.CharacterStateInfoSPtr->GetRemainTime() < 0.f)
		{
			MoveSpeedOffsetMap.Remove(Iter.Key);

			auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

			GAEventDataPtr->TriggerCharacterPtr = Iter.Value.SPtr->TriggerCharacterPtr;

			FGAEventData GAEventData(Iter.Value.SPtr->TargetCharacterPtr, Iter.Value.SPtr->TriggerCharacterPtr);

			GAEventData.DataSource = Iter.Value.SPtr->Tag;
			GAEventData.DataModify = { {ECharacterPropertyType::MoveSpeed,0 } };
			GAEventData.bIsClearData = true;

			GAEventDataPtr->DataAry.Add(GAEventData);

			CharacterPtr->GetBaseFeatureComponent()->SendEventImp(GAEventDataPtr);

			CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(Iter.Value.CharacterStateInfoSPtr);
		}
		else
		{
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(Iter.Value.CharacterStateInfoSPtr);
		}
	}

	if (MoveSpeedOffsetMap.IsEmpty())
	{
		K2_CancelAbility();
		return;
	}

	for (const auto Iter : MoveSpeedOffsetMap)
	{
		auto GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent;

		GAEventDataPtr->TriggerCharacterPtr = Iter.Value.SPtr->TriggerCharacterPtr;

		FGAEventData GAEventData(Iter.Value.SPtr->TargetCharacterPtr, Iter.Value.SPtr->TriggerCharacterPtr);

		GAEventData.DataSource = Iter.Value.SPtr->Tag;
		GAEventData.DataModify = { {ECharacterPropertyType::MoveSpeed,Iter.Key } };
		GAEventData.bIsOverlapData = true;

		GAEventDataPtr->DataAry.Add(GAEventData);

		CharacterPtr->GetBaseFeatureComponent()->SendEventImp(GAEventDataPtr);
		break;
	}
}
