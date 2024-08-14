// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include <EnvironmentQuery/EnvQueryTypes.h>
#include <AITypes.h>
#include <Navigation/PathFollowingComponent.h>

#include "BasicFuturesBase.h"
#include "GenerateType.h"

#include "BasicFutures_MoveToAttaclArea.generated.h"

struct FEnvQueryResult;
class UEnvQuery;

struct FCanbeInteractionInfo;

struct FGameplayAbilityTargetData_MoveToAttaclArea : public FGameplayAbilityTargetData
{
	ACharacterBase*TargetCharacterPtr = nullptr;

	FGameplayAbilityTargetData* DataPtr = nullptr;

	int32 AttackDistance = 100;

	TSharedPtr<FCanbeInteractionInfo> CanbeActivedInfoSPtr;
};

UCLASS()
class PLANET_API UBasicFutures_MoveToAttaclArea : public UBasicFuturesBase
{
	GENERATED_BODY()

public:

	virtual void PostCDOContruct() override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	)override;

protected:

	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	UFUNCTION()
	void MoveCompletedSignature(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UPROPERTY(EditAnywhere, Category = Param)
	TObjectPtr<UEnvQuery> QueryTemplate;

	UPROPERTY(EditAnywhere, Category = Param)
	int32 MinDistance = 50;
	
	UPROPERTY(EditAnywhere, Category = Param)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode = EEnvQueryRunMode::SingleResult;

private:

	int32 RequestID = -1;

	ACharacterBase* AvatorCharacterPtr = nullptr;

	TSharedPtr<FCanbeInteractionInfo> CanbeActivedInfoSPtr;
};
