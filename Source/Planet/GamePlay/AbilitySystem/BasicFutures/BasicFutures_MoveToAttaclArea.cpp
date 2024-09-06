// Copyright 2020 Dan Kestranek.

#include "BasicFutures_MoveToAttaclArea.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Kismet/KismetMathLibrary.h"
#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "UnitProxyProcessComponent.h"

#include "BaseFeatureGAComponent.h"
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

void UBasicFutures_MoveToAttaclArea::PostCDOContruct()
{
	Super::PostCDOContruct();

	if (GetWorldImp())
	{
		AbilityTags.AddTag(UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea);
		ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea);

		FAbilityTriggerData AbilityTriggerData;

		AbilityTriggerData.TriggerTag = UGameplayTagsSubSystem::GetInstance()->State_MoveToAttaclArea;
		AbilityTriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

		AbilityTriggers.Add(AbilityTriggerData);
	}
}

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
		auto DataPtr = dynamic_cast<const FGameplayAbilityTargetData_MoveToAttaclArea*>(TriggerEventData->TargetData.Get(0));
		if (DataPtr)
		{
			CanbeActivedInfoSPtr = DataPtr->CanbeActivedInfoSPtr;

			AvatorCharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

			FEnvQueryRequest QueryRequest(QueryTemplate, AvatorCharacterPtr);

			QueryRequest.SetFloatParam(FBasicFutures_MoveToAttaclArea::Get().Donut_InnerRadius, MinDistance);
			QueryRequest.SetFloatParam(
				FBasicFutures_MoveToAttaclArea::Get().Donut_OuterRadius, DataPtr->AttackDistance - UGameOptions::GetInstance()->MoveToAttaclAreaOffset
			);

			auto QueryFinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &ThisClass::OnQueryFinished);
			RequestID = QueryRequest.Execute(RunMode, QueryFinishedDelegate);
		}
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
			PCPtr->StopMovement();
			PCPtr->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::MoveCompletedSignature);
		}
		else
		{

		}
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UBasicFutures_MoveToAttaclArea::OnQueryFinished(TSharedPtr<FEnvQueryResult> ResultSPtr)
{
	if (ResultSPtr->IsAborted())
	{
		K2_CancelAbility();

		return;
	}

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

			const auto Result = PCPtr->MoveToLocation(DestLocation);
			if (Result == EPathFollowingRequestResult::RequestSuccessful)
			{
				PCPtr->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::MoveCompletedSignature);
			}
			else
			{
				K2_CancelAbility();
			}
		}
		else
		{

		}
	}
}

void UBasicFutures_MoveToAttaclArea::MoveCompletedSignature(FAIRequestID InRequestID, EPathFollowingResult::Type Result)
{
	ON_SCOPE_EXIT
	{
		K2_CancelAbility();
	};

	if (Result == EPathFollowingResult::Success)
	{
	//	AvatorCharacterPtr->GetInteractiveSkillComponent()->ActiveAction(CanbeActivedInfoSPtr);
	}
	else
	{
	}
}
