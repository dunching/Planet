// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "QuestSystemStateTreeComponent.h"
#include "QuestChainBase.h"
#include "QuestChainType.h"

#include "QuestChain.generated.h"

class UGameplayTasksComponent;

class UQuestSubSystem;
class UQuestSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class APlanetPlayerController;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

struct FTaskNodeDescript;
struct FSTT_GuideThread_Completet;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

#pragma region Base
/**
 *	任务
 *	仅在 ROLE_AutonomousProxy 执行
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AQuestChain : public AQuestChainBase
{
	GENERATED_BODY()

	using FOnGuideThreadNameChagned = TMulticastDelegate<void(
		const FString&
		
		)>;

public:
	friend FSTT_GuideThread_Completet;
	friend UQuestSubSystem;

	AQuestChain(
		const FObjectInitializer& ObjectInitializer
		);
};
#pragma endregion

#pragma region 主线
/**
 *	主线任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AQuestChain_Main : public AQuestChain_MainBase
{
	GENERATED_BODY()

public:
	AQuestChain_Main(
		const FObjectInitializer& ObjectInitializer
		);
};

UCLASS()
class UStateTreeGuideMainThreadComponentSchema : public USTQuestChainComponentSchemaBase
{
	GENERATED_BODY()

public:
	using FOwnerType = AQuestChain_Main;

	UStateTreeGuideMainThreadComponentSchema();
};

/**
 *
 */
UCLASS()
class PLANET_API UGuideMainThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponentBase
{
	GENERATED_BODY()

public:
	using FSchemaType = UStateTreeGuideMainThreadComponentSchema;

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion

#pragma region 支线
/**
 *	支线任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AQuestChain_Branch : public AQuestChain_BranchBase
{
	GENERATED_BODY()

public:
};
#pragma endregion

#pragma region 即时任务
/**
 *	即时事件，如即时挑战
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AQuestChain_Immediate : public AQuestChain_ImmediateBase
{
	GENERATED_BODY()

public:
};

#pragma endregion

#pragma region 区域事件
/**
 *	区域事件，进入区域后发生的引导,如世界首领、副本
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AQuestChain_Area : public AQuestChain_AreaBase
{
	GENERATED_BODY()

public:
	AQuestChain_Area(
		const FObjectInitializer& ObjectInitializer
		);
};

UCLASS()
class UStateTreeGuideAreaThreadComponentSchema : public USTQuestChainComponentSchemaBase
{
	GENERATED_BODY()

public:
	using FOwnerType = AQuestChain_Area;

	UStateTreeGuideAreaThreadComponentSchema();
};

/**
 *
 */
UCLASS()
class PLANET_API UGuideAreaThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponentBase
{
	GENERATED_BODY()

public:
	using FSchemaType = UStateTreeGuideAreaThreadComponentSchema;

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion
