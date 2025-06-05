// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BasicFutures_MoveToAttaclArea.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"
#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "ProxyProcessComponent.h"

#include "CharacterAbilitySystemComponent.h"
#include "UICommon.h"
#include "PlanetPlayerController.h"
#include "GameOptions.h"
#include "Planet_Tools.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> DrawDebugSTT_MoveToAttaclArea(
	TEXT("Skill.DrawDebug.MoveToAttaclArea"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

struct FBasicFutures_MoveToAttaclArea : public TStructVariable<FBasicFutures_MoveToAttaclArea>
{
	FName Donut_InnerRadius = TEXT("Donut.InnerRadius");

	FName Donut_OuterRadius = TEXT("Donut.OuterRadius");
};

UBasicFutures_MoveToAttaclArea::UBasicFutures_MoveToAttaclArea() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

// void UBasicFutures_MoveToAttaclArea::InitalDefaultTags()
// {
// 	Super::InitalDefaultTags();
//
// 	if (GetWorldImp())
// 	{
// 		// // AbilityTags.AddTag(UGameplayTagsLibrary::State_MoveToAttaclArea);
// 		
// 		ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_MoveToAttaclArea);
// 	}
// }

void UBasicFutures_MoveToAttaclArea::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetData_MoveToAttaclAreaSPtr =
			dynamic_cast<const FGameplayAbilityTargetData_MoveToAttaclArea*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetData_MoveToAttaclAreaSPtr)
		{
			AvatorCharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

			FEnvQueryRequest QueryRequest(QueryTemplate, AvatorCharacterPtr);

			QueryRequest.SetFloatParam(FBasicFutures_MoveToAttaclArea::Get().Donut_InnerRadius, MinDistance);
			QueryRequest.SetFloatParam(
				FBasicFutures_MoveToAttaclArea::Get().Donut_OuterRadius,
				GameplayAbilityTargetData_MoveToAttaclAreaSPtr->AttackDistance - UGameOptions::GetInstance()->MoveToAttaclAreaOffset
			);

			auto QueryFinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &ThisClass::OnQueryFinished);
			RequestID = QueryRequest.Execute(RunMode, QueryFinishedDelegate);
		}
	}

	if (RequestID == INDEX_NONE)
	{
		OnQueryFinished(nullptr);
	}
}

bool UBasicFutures_MoveToAttaclArea::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UBasicFutures_MoveToAttaclArea::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(AvatorCharacterPtr);

	if (QueryManager)
	{
		QueryManager->AbortQuery(RequestID);
	}

	if (AvatorCharacterPtr)
	{
		if (AvatorCharacterPtr->IsPlayerControlled())
		{
			auto PCPtr = AvatorCharacterPtr->GetController<APlanetPlayerController>();
			if (PCPtr)
			{
				PCPtr->StopMovement();
				PCPtr->ReceiveMoveCompleted.BindUObject(this, &ThisClass::MoveCompletedSignature);
			}
		}
		else
		{
			auto PCPtr = AvatorCharacterPtr->GetController<AAIController>();
			if (PCPtr)
			{
				PCPtr->StopMovement();
				PCPtr->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::MoveCompletedSignature);
			}
		}
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UBasicFutures_MoveToAttaclArea::OnQueryFinished(TSharedPtr<FEnvQueryResult> ResultSPtr)
{
	if (ResultSPtr)
	{
		switch (ResultSPtr->GetRawStatus())
		{
		case EEnvQueryStatus::Success:
		{
			const auto DestLocation = ResultSPtr->GetItemAsLocation(0);
#ifdef WITH_EDITOR
			if (DrawDebugSTT_MoveToAttaclArea.GetValueOnGameThread())
			{
				DrawDebugSphere(GetWorld(), DestLocation, 20, 20, FColor::Yellow, false, 5);
			}
#endif

			if (AvatorCharacterPtr)
			{
				if (AvatorCharacterPtr->IsPlayerControlled())
				{
					auto PCPtr = AvatorCharacterPtr->GetController<APlanetPlayerController>();

					PCPtr->MoveToLocation(DestLocation, nullptr);
				}
				else
				{

				}
			}
		}
		break;
		default:
		{
		}
		break;
		}
	}

	MoveCompletedSignature(RequestID, EPathFollowingResult::Invalid);

	K2_CancelAbility();
}

void UBasicFutures_MoveToAttaclArea::MoveCompletedSignature(FAIRequestID InRequestID, EPathFollowingResult::Type Result)
{
	ON_SCOPE_EXIT
	{
		K2_CancelAbility();
	};

	if (Result == EPathFollowingResult::Success)
	{
		//	AvatorCharacterPtr->GetProxyProcessComponent()->ActiveAction(CanbeActivedInfoSPtr);
	}
	else
	{
	}

	if (GameplayAbilityTargetData_MoveToAttaclAreaSPtr)
	{
		std::ignore = GameplayAbilityTargetData_MoveToAttaclAreaSPtr->MoveCompletedSignature(Result);
		GameplayAbilityTargetData_MoveToAttaclAreaSPtr = nullptr;
	}
}