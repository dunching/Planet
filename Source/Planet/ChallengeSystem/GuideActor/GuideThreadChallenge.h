// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideThread.h"

#include "GuideThreadChallenge.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

struct FTaskNodeDescript;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

/**
 *	挑战/爬塔任务
 *	随机选择关卡？
 *	根据关卡的不同，生成不同数量和类型的敌人
 *	不断重复并且强化
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideThread_Challenge : public AGuideThread
{
	GENERATED_BODY()

public:
	AGuideThread_Challenge(const FObjectInitializer& ObjectInitializer);

};

UCLASS()
class UStateTreeGuideChallengeThreadComponentSchema : public UStateTreeGuideThreadComponentSchema
{
	GENERATED_BODY()

public:
	
	using FOwnerType = AGuideThread_Challenge;

	UStateTreeGuideChallengeThreadComponentSchema();
	
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
	
};

/**
 *
 */
UCLASS()
class PLANET_API UGuideChallengeThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UStateTreeSchema>GetSchema() const override;
	
	virtual bool SetContextRequirements(FStateTreeExecutionContext& Context, bool bLogErrors = false) override;
};