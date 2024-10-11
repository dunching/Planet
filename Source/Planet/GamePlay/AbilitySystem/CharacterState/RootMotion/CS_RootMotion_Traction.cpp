
#include "CS_RootMotion_Traction.h"

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
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "AbilityTask_MyApplyRootMotionRadialForce.h"
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_RootMotion_Traction::FGameplayAbilityTargetData_RootMotion_Traction() :
	Super(UGameplayTagsSubSystem::GetInstance()->State_RootMotion_Traction)
{

}

FGameplayAbilityTargetData_RootMotion_Traction* FGameplayAbilityTargetData_RootMotion_Traction::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_Traction;

	*ResultPtr = *this;

	return ResultPtr;
}

void UCS_RootMotion_Traction::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_RootMotion_Traction::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FRootMotionParam*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			SetCache(TSharedPtr<FRootMotionParam>(GameplayAbilityTargetDataPtr->Clone()));
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion_Traction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_Traction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	//
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_RootMotion_Traction::UpdateDuration()
{
	Super::UpdateDuration();

	if (CharacterStateInfoSPtr)
	{
		CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
		CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
		CharacterStateInfoSPtr->RefreshTime();
		CharacterStateInfoSPtr->DataChanged();
	}

	if (GameplayAbilityTargetDataSPtr && GameplayAbilityTargetDataSPtr->bIsEnd)
	{
		OnTaskComplete();
	}
	else if (RootMotionTaskPtr)
	{
		RootMotionTaskPtr->UpdateLocation(GameplayAbilityTargetDataSPtr->TaretPt);
	}
}

void UCS_RootMotion_Traction::SetCache(const TSharedPtr<FRootMotionParam>& InGameplayAbilityTargetDataSPtr)
{
	GameplayAbilityTargetDataSPtr = InGameplayAbilityTargetDataSPtr;
}

void UCS_RootMotion_Traction::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UCS_RootMotion_Traction::ExcuteTasks()
{
	// 
	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

	// 
	RootMotionTaskPtr = UAbilityTask_MyApplyRootMotionRadialForce::MyApplyRootMotionRadialForce(
		this,
		TEXT(""),
		GameplayAbilityTargetDataSPtr->TaretPt,
		nullptr,
		GameplayAbilityTargetDataSPtr->MoveSpeed,
		-1,
		GameplayAbilityTargetDataSPtr->Radius,
		false,
		true,
		true,
		nullptr,
		nullptr,
		false,
		FRotator::ZeroRotator,
		ERootMotionFinishVelocityMode::ClampVelocity,
		FVector::ZeroVector,
		200
	);

	RootMotionTaskPtr->OnFinish.BindUObject(this, &ThisClass::OnTaskComplete);
	RootMotionTaskPtr->ReadyForActivation();
}

void UCS_RootMotion_Traction::OnTaskComplete()
{
	K2_CancelAbility();
}
