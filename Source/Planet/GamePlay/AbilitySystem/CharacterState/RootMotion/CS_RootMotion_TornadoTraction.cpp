
#include "CS_RootMotion_TornadoTraction.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/CapsuleComponent.h>

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

ATornado::ATornado(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/) :
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	CapsuleComponentPtr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponentPtr->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponentPtr->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponentPtr->CanCharacterStepUpOn = ECB_No;
	CapsuleComponentPtr->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponentPtr->SetCanEverAffectNavigation(false);
	CapsuleComponentPtr->bDynamicObstacle = true;
	CapsuleComponentPtr->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicatingMovement(true);
}

FGameplayAbilityTargetData_RootMotion_TornadoTraction::FGameplayAbilityTargetData_RootMotion_TornadoTraction() :
	Super(UGameplayTagsLibrary::TornadoTraction)
{

}

FGameplayAbilityTargetData_RootMotion_TornadoTraction* FGameplayAbilityTargetData_RootMotion_TornadoTraction::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_TornadoTraction;

	*ResultPtr = *this;

	return ResultPtr;
}

void UCS_RootMotion_TornadoTraction::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_RootMotion_TornadoTraction*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion_TornadoTraction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_TornadoTraction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (EffectItemPtr)
	{
		EffectItemPtr->RemoveFromParent();
		EffectItemPtr = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// void UCS_RootMotion_TornadoTraction::UpdateDurationImp()
// {
// 	Super::UpdateDurationImp();
// 
// 	if (TaskPtr)
// 	{
// 		TaskPtr->UpdateDuration();
// 	}
// 	if (GameplayAbilityTargetDataPtr->Tag.MatchesTagExact(UGameplayTagsLibrary::FlyAway))
// 	{
// 		for (auto Iter : ActiveTasks)
// 		{
// 			if (Iter->IsA(UAbilityTask_FlyAway::StaticClass()))
// 			{
// 				auto RootMotionTaskPtr = Cast<UAbilityTask_FlyAway>(Iter);
// 
// 				RootMotionTaskPtr->UpdateDuration();
// 				break;
// 			}
// 		}
// 	}
// }

void UCS_RootMotion_TornadoTraction::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();

		auto RootMotionTaskPtr = UAbilityTask_Tornado::NewTask(
			this,
			TEXT(""),
			GameplayAbilityTargetDataPtr->TornadoPtr.Get(),
			GameplayAbilityTargetDataPtr->TargetCharacterPtr.Get()
		);

		RootMotionTaskPtr->Ability = this;
		RootMotionTaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		RootMotionTaskPtr->OnFinish.BindUObject(this, &ThisClass::OnTaskComplete);

		RootMotionTaskPtr->ReadyForActivation();
	}
}

void UCS_RootMotion_TornadoTraction::InitalDefaultTags()
{
	Super::InitalDefaultTags();
}

void UCS_RootMotion_TornadoTraction::ExcuteTasks()
{
	if (CharacterPtr->IsPlayerControlled())
	{
	}
	else
	{

	}
}

void UCS_RootMotion_TornadoTraction::OnTaskComplete()
{
	K2_CancelAbility();
}

void UCS_RootMotion_TornadoTraction::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UCS_RootMotion_TornadoTraction::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CharacterPtr->IsPlayerControlled())
	{
		if (CurrentInterval > Interval)
		{
		}
		else
		{
			if (EffectItemPtr)
			{
			}
		}
	}
	else
	{

	}
}
