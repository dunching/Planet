
#include "Skill_Active_Traction.h"

#include <Engine/OverlapResult.h>

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "CS_RootMotion.h"
#include "GameplayTagsSubSystem.h"
#include "BaseFeatureComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "KismetCollisionHelper.h"
#include "CS_RootMotion_Traction.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

void USkill_Active_Traction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);

		CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		CharacterStateInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
		CharacterStateInfoSPtr->Duration = Duration;
		CharacterStateInfoSPtr->DefaultIcon = SkillUnitPtr->GetIcon();
		CharacterStateInfoSPtr->DataChanged();

		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
			TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::IntervalDelegate, false);
			TaskPtr->OnFinished.BindLambda([this](auto)
				{
					IntervalDelegate(nullptr, 0.f, 0.f, true);

					K2_CancelAbility();
					return true;
				});
			TaskPtr->ReadyForActivation();
		}

		IntervalDelegate(nullptr, 0.f, 0.f, false);
	}
#endif
}

void USkill_Active_Traction::EndAbility(
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

void USkill_Active_Traction::IntervalDelegate(
	UAbilityTask_TimerHelper*,
	float CurrentIntervalTime, 
	float IntervalTime,
	bool bIsEnd
)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CurrentIntervalTime >= IntervalTime)
		{
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(CharacterPtr);

			TArray<FOverlapResult> Result;
			CharacterPtr->GetWorld()->OverlapMultiByObjectType(
				Result,
				CharacterPtr->GetActorLocation(),
				FQuat::Identity,
				ObjectQueryParams,
				FCollisionShape::MakeSphere(Radius),
				Params
			);

			TSet<ACharacterBase*>TargetSet;
			for (const auto& Iter : Result)
			{
				auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
				if (TargetCharacterPtr && !CharacterPtr->IsGroupmate(TargetCharacterPtr))
				{
					TargetSet.Add(TargetCharacterPtr);
				}
			}

			auto ICPtr = CharacterPtr->GetBaseFeatureComponent();

			// 控制效果
			for (const auto& Iter : TargetSet)
			{
				auto GameplayAbilityTargetData_StateModifyPtr = new FGameplayAbilityTargetData_RootMotion_Traction;

				GameplayAbilityTargetData_StateModifyPtr->TriggerCharacterPtr = CharacterPtr;
				GameplayAbilityTargetData_StateModifyPtr->TargetCharacterPtr = Iter;

				GameplayAbilityTargetData_StateModifyPtr->MoveSpeed = MoveSpeed;
				GameplayAbilityTargetData_StateModifyPtr->Radius = Radius;
				GameplayAbilityTargetData_StateModifyPtr->TaretPt = CharacterPtr->GetActorLocation();
				GameplayAbilityTargetData_StateModifyPtr->bIsEnd = bIsEnd;

				ICPtr->SendEventImp(GameplayAbilityTargetData_StateModifyPtr);
			}
		}
	}
#endif
}

void USkill_Active_Traction::DurationDelegate(UAbilityTask_TimerHelper*, float CurrentIntervalTime, float IntervalTime)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = CurrentIntervalTime;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
		}
	}
#endif
}
