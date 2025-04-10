
#include "Skill_Active_PropertySettlementModify.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Components/SplineComponent.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include <Engine/OverlapResult.h>
#include <GameFramework/SpringArmComponent.h>
#include "Net/UnrealNetwork.h"


#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_TimerHelper.h"
#include "Helper_RootMotionSource.h"
#include "AbilityTask_tornado.h"
#include "CS_RootMotion.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"

void USkill_Active_PropertySettlementModify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		struct FMyPropertySettlementModify : public FPropertySettlementModify
		{
			FMyPropertySettlementModify():
				FPropertySettlementModify(10)
			{
			
			}

			virtual int32 SettlementModify(const TMap<FGameplayTag, int32>& ValueMap)const override
			{
				const auto Result = FPropertySettlementModify::SettlementModify(ValueMap);
				return 100 < Result ? 100 : Result;
			}
		};

		// const auto MyPropertySettlementModify = MakeShared<FMyPropertySettlementModify>();
		// CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().MoveSpeed.AddSettlementModify(MyPropertySettlementModify);
		//
		// auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		// TaskPtr->SetDuration(Duration);
		// TaskPtr->OnFinished.BindLambda([MyPropertySettlementModify, this](auto)
		// 	{
		// 		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().MoveSpeed.RemoveSettlementModify(MyPropertySettlementModify);
		//
		// 		CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
		//
		// 		K2_CancelAbility();
		// 		return true;
		// 	});
		// TaskPtr->ReadyForActivation();
	}
#endif
}
