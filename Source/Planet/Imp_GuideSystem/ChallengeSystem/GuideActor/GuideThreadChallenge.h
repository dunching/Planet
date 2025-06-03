// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideThread.h"
#include "OpenWorldDataLayer.h"

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
class PLANET_API AGuideThread_Challenge : public AGuideThread_Immediate
{
	GENERATED_BODY()

public:
	
	/**
	 * 当前在哪个挑战关卡入口
	 */
	ETeleport CurrentTeleport = ETeleport::kChallenge_LevelType_1;
	
	/**
	 * 每多少层进入一个指定关卡
	 * 如同LOL斗魂，指定关卡层数进入选秀或选择强化关卡
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, ETeleport> SpecifySpecialLevels;
	
	AGuideThread_Challenge(const FObjectInitializer& ObjectInitializer);

	virtual FString GetGuideThreadTitle() const;

	void SetCurrentLevel(int32 NewCurrentLevel );
	
	int32 GetCurrentLevel()const;
	
protected:
	
	/**
	 * 当前层数
	 */
	int32 CurrentLevel = 0;

};

UCLASS()
class UStateTreeGuideChallengeThreadComponentSchema : public UStateTreeGuideThreadComponentSchemaBase
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
class PLANET_API UGuideChallengeThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponentBase
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UStateTreeSchema>GetSchema() const override;
	
	virtual bool SetContextRequirements(FStateTreeExecutionContext& Context, bool bLogErrors = false) override;
};