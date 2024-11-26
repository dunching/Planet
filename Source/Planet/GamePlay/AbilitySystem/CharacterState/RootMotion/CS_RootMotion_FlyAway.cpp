
#include "CS_RootMotion_FlyAway.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

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
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_RootMotion_FlyAway::FGameplayAbilityTargetData_RootMotion_FlyAway() :
	Super(UGameplayTagsSubSystem::FlyAway)
{

}

UScriptStruct* FGameplayAbilityTargetData_RootMotion_FlyAway::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_RootMotion_FlyAway::StaticStruct();
}

bool FGameplayAbilityTargetData_RootMotion_FlyAway::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Duration;
	Ar << Height;

	return true;
}

FGameplayAbilityTargetData_RootMotion_FlyAway* FGameplayAbilityTargetData_RootMotion_FlyAway::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_FlyAway;

	*ResultPtr = *this;

	return ResultPtr;
}

void UCS_RootMotion_FlyAway::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (Spec.GameplayEventData)
	{
		GameplayAbilityTargetDataSPtr =
			MakeSPtr_GameplayAbilityTargetData<FRootMotionParam>(Spec.GameplayEventData->TargetData.Get(0));
	}
}

void UCS_RootMotion_FlyAway::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion_FlyAway::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_FlyAway::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	//
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);
	CharacterStateInfoSPtr = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_RootMotion_FlyAway::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantRotation);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_Orient2Acce);
}

void UCS_RootMotion_FlyAway::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UCS_RootMotion_FlyAway::UpdateRootMotionImp(const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& DataSPtr)
{
	Super::UpdateRootMotionImp(DataSPtr);

	GameplayAbilityTargetDataSPtr = DynamicCastSharedPtr<FGameplayAbilityTargetData_RootMotion_FlyAway>(DataSPtr);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
			CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
			CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
			CharacterStateInfoSPtr->RefreshTime();
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);

			if (AbilityTask_TimerHelperPtr)
			{
				AbilityTask_TimerHelperPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
				AbilityTask_TimerHelperPtr->UpdateDuration();
			}
		}
		else 
		{
			// 
			CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
			CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
			CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
			CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

			// 
			AbilityTask_TimerHelperPtr = UAbilityTask_TimerHelper::DelayTask(this);
			AbilityTask_TimerHelperPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
			AbilityTask_TimerHelperPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
			AbilityTask_TimerHelperPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
			AbilityTask_TimerHelperPtr->ReadyForActivation();
		}
	}
#endif

	if (RootMotionTaskPtr)
	{
		RootMotionTaskPtr->UpdateDuration(GameplayAbilityTargetDataSPtr->Height, GameplayAbilityTargetDataSPtr->Duration);
	}
}

void UCS_RootMotion_FlyAway::ExcuteTasks()
{
	if (CharacterPtr->IsPlayerControlled())
	{
	}
	else
	{

	}

	if (GameplayAbilityTargetDataSPtr->Duration < 0.f)
	{
	}
	else
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			// 
			CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
			CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
			CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
			CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

			// 
			AbilityTask_TimerHelperPtr = UAbilityTask_TimerHelper::DelayTask(this);
			AbilityTask_TimerHelperPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
			AbilityTask_TimerHelperPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
			AbilityTask_TimerHelperPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
			AbilityTask_TimerHelperPtr->ReadyForActivation();
		}
#endif

		// 
		RootMotionTaskPtr = UAbilityTask_FlyAway::NewTask(
			this,
			TEXT(""),
			GameplayAbilityTargetDataSPtr->Duration,
			GameplayAbilityTargetDataSPtr->Height
		);

		RootMotionTaskPtr->OnFinished.BindUObject(this, &ThisClass::K2_CancelAbility);
		RootMotionTaskPtr->ReadyForActivation();
	}
}

void UCS_RootMotion_FlyAway::OnTaskComplete()
{
	K2_CancelAbility();
}

void UCS_RootMotion_FlyAway::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UCS_RootMotion_FlyAway::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval > Interval)
	{
		// 进入下落的状态

// 		CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);
// 		CharacterStateInfoSPtr = nullptr;
	}
	else
	{
		CharacterStateInfoSPtr->TotalTime = CurrentInterval;

		CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
	}
}
