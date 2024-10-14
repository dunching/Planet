
#include "Skill_Active_Slow.h"

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

#include "GAEvent_Helper.h"
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
#include "GameplayTagsSubSystem.h"
#include "BaseFeatureComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "KismetCollisionHelper.h"
#include "CS_PeriodicStateModify_Slow.h"

void USkill_Active_Slow::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(CharacterPtr);

		const auto Dir = UKismetMathLibrary::MakeRotFromZX(
			UKismetGravityLibrary::GetGravity(CharacterPtr->GetActorLocation()), CharacterPtr->GetControlRotation().Vector()
		).Vector();

		auto Result = UKismetCollisionHelper::OverlapMultiSectorByObjectType(
			GetWorld(),
			CharacterPtr->GetActorLocation(),
			CharacterPtr->GetActorLocation() + (Dir * Radius),
			Angle,
			9,
			ObjectQueryParams,
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
			auto GameplayAbilityTargetData_StateModifyPtr = new FGameplayAbilityTargetData_StateModify_Slow(
				-MoveSpeedOffset
			);

			GameplayAbilityTargetData_StateModifyPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetData_StateModifyPtr->TargetCharacterPtr = Iter;

			ICPtr->SendEventImp(GameplayAbilityTargetData_StateModifyPtr);
		}

		CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());

		K2_CancelAbility();
	}
#endif
}
