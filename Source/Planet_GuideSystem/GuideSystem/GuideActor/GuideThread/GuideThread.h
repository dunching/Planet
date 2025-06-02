// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideSystemStateTreeComponent.h"
#include "GuideThreadBase.h"
#include "GuideThreadType.h"

#include "GuideThread.generated.h"


class UGameplayTasksComponent;

class UGuideSubSystem;
class UGuideSystemStateTreeComponent;
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
class PLANET_GUIDESYSTEM_API AGuideThread : public AGuideThreadBase
{
	GENERATED_BODY()

	using FOnGuideThreadNameChagned = TMulticastDelegate<void(
		const FString&
		
		)>;

public:
	friend FSTT_GuideThread_Completet;
	friend UGuideSubSystem;

	AGuideThread(
		const FObjectInitializer& ObjectInitializer
		);
};
#pragma endregion

#pragma region 主线
/**
 *	主线任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_GUIDESYSTEM_API AGuideThread_Main : public AGuideThread_MainBase
{
	GENERATED_BODY()

public:
	AGuideThread_Main(
		const FObjectInitializer& ObjectInitializer
		);
};

UCLASS()
class UStateTreeGuideMainThreadComponentSchema : public UStateTreeGuideThreadComponentSchemaBase
{
	GENERATED_BODY()

public:
	using FOwnerType = AGuideThread_Main;

	UStateTreeGuideMainThreadComponentSchema();
};

/**
 *
 */
UCLASS()
class PLANET_GUIDESYSTEM_API UGuideMainThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponentBase
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
class PLANET_GUIDESYSTEM_API AGuideThread_Branch : public AGuideThread_BranchBase
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
class PLANET_GUIDESYSTEM_API AGuideThread_Immediate : public AGuideThread_ImmediateBase
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
class PLANET_GUIDESYSTEM_API AGuideThread_Area : public AGuideThread_AreaBase
{
	GENERATED_BODY()

public:
	AGuideThread_Area(
		const FObjectInitializer& ObjectInitializer
		);
};

UCLASS()
class UStateTreeGuideAreaThreadComponentSchema : public UStateTreeGuideThreadComponentSchemaBase
{
	GENERATED_BODY()

public:
	using FOwnerType = AGuideThread_Area;

	UStateTreeGuideAreaThreadComponentSchema();
};

/**
 *
 */
UCLASS()
class PLANET_GUIDESYSTEM_API UGuideAreaThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponentBase
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
