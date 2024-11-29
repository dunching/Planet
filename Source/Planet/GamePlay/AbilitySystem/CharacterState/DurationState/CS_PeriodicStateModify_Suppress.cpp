
#include "CS_PeriodicStateModify_Suppress.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"

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
#include "GameplayTagsLibrary.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_PlayMontage.h"

static TAutoConsoleVariable<int32> CS_PeriodicStateModify_Suppress(
	TEXT("CS_PeriodicStateModify_Suppress"),
	0,
	TEXT("")
	TEXT(" default: 0"));

FGameplayAbilityTargetData_StateModify_Suppress::FGameplayAbilityTargetData_StateModify_Suppress(
	float InDuration
) :
	Super(UGameplayTagsLibrary::State_Debuff_Suppress, InDuration)
{
}

FGameplayAbilityTargetData_StateModify_Suppress::FGameplayAbilityTargetData_StateModify_Suppress(
	UAnimMontage* InHumanMontagePtr
) :
	Super(UGameplayTagsLibrary::State_Debuff_Suppress, InHumanMontagePtr->CalculateSequenceLength())
{
	HumanMontagePtr = InHumanMontagePtr;
}

FGameplayAbilityTargetData_StateModify_Suppress::FGameplayAbilityTargetData_StateModify_Suppress()
{

}

UScriptStruct* FGameplayAbilityTargetData_StateModify_Suppress::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_StateModify_Suppress::StaticStruct();
}

bool FGameplayAbilityTargetData_StateModify_Suppress::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << HumanMontagePtr;

	return true;
}

FGameplayAbilityTargetData_StateModify_Suppress* FGameplayAbilityTargetData_StateModify_Suppress::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_StateModify_Suppress;

	*ResultPtr = *this;

	return ResultPtr;
}

TSharedPtr<FGameplayAbilityTargetData_StateModify_Suppress> FGameplayAbilityTargetData_StateModify_Suppress::Clone_SmartPtr() const
{
	return TSharedPtr<FGameplayAbilityTargetData_StateModify_Suppress>(Clone());
}

UCS_PeriodicStateModify_Suppress::UCS_PeriodicStateModify_Suppress() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UCS_PeriodicStateModify_Suppress::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (Spec.GameplayEventData)
	{
		StateParamSPtr =
			MakeSPtr_GameplayAbilityTargetData<FStateParam>(Spec.GameplayEventData->TargetData.Get(0));
	}
}

void UCS_PeriodicStateModify_Suppress::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Suppress::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Suppress::UpdateDuration()
{
	Super::UpdateDuration();

	if (TimerTaskPtr)
	{
		TimerTaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		TimerTaskPtr->UpdateDuration();
	}

	if (CharacterStateInfoSPtr)
	{
		CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
		CharacterStateInfoSPtr->TotalTime = 0.f;
	}
}

void UCS_PeriodicStateModify_Suppress::PerformAction()
{
	Super::PerformAction();

	if (StateParamSPtr)
	{
#if UE_EDITOR || UE_SERVER
		if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
			CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
			if (StateParamSPtr->HumanMontagePtr)
			{
				CharacterStateInfoSPtr->Duration = StateParamSPtr->HumanMontagePtr->CalculateSequenceLength();
			}
			else
			{
				CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
			}
			CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

		}
#endif
		const auto Z = -UKismetGravityLibrary::GetGravity();
		const auto Dir =
			StateParamSPtr->TriggerCharacterPtr->GetActorLocation() -
			CharacterPtr->GetActorLocation();
		const auto FocusRotation = UKismetMathLibrary::MakeRotFromZX(Z, Dir);
		
		CharacterPtr->FaceRotation(FocusRotation);

#ifdef WITH_EDITOR
		if (CS_PeriodicStateModify_Suppress.GetValueOnGameThread())
		{
			DrawDebugLine(
				GetWorld(), 
				CharacterPtr->GetActorLocation(),
				CharacterPtr->GetActorLocation() + (FocusRotation.Vector() * 200),
				FColor::Red, 
				false, 
				10
			);
		}
#endif

		PlayMontage();
	}
	else
	{
		checkNoEntry();
		K2_CancelAbility();
	}
}

void UCS_PeriodicStateModify_Suppress::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CharacterStateInfoSPtr->TotalTime += DeltaTime;

		CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
	}
#endif
}

void UCS_PeriodicStateModify_Suppress::PlayMontage()
{
	if (StateParamSPtr && StateParamSPtr->HumanMontagePtr)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			StateParamSPtr->HumanMontagePtr,
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}

void UCS_PeriodicStateModify_Suppress::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsLibrary::State_Debuff_Suppress);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_ReleasingSkill);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Debuff_Suppress);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPathFollowMove);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRotation);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_UseCustomRotation);
}
