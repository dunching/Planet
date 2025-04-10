// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideThread.h"

#include "GuideThreadChallengeActor.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

struct FTaskNodeDescript;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

UCLASS()
class UStateTreeGuideThreadChallengeComponentSchema : public UStateTreeComponentSchema
{
	GENERATED_BODY()

public:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
};

class UStateTreeComponent;

/**
 *
 */
UCLASS()
class PLANET_API UGuideChallengeSystemStateTreeComponent : public UGuideSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;
};

/**
 *	挑战/爬塔任务
 *	随机选择关卡？
 *	根据关卡的不同，生成不同数量和类型的敌人
 *	不断重复并且强化
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideChallengeThread : public AGuideThread
{
	GENERATED_BODY()

public:
	AGuideChallengeThread(const FObjectInitializer& ObjectInitializer);

};
