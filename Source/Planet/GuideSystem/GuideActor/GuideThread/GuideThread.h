// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "GuideActor.h"
#include "GuideSystemStateTreeComponent.h"
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

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGuideThreadInterfacetion : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	virtual EGuideThreadType GetGuideThreadType() const = 0;

private:
};

#pragma region Base
/**
 *	任务
 *	仅在 ROLE_AutonomousProxy 执行
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideThread : public AGuideActor
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

	void UpdateCurrentTaskNode(
		const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode
		);

	void UpdateCurrentTaskNode(
		const FTaskNodeDescript& TaskNodeDescript
		);

	// 添加任务执行结果
	void AddEvent(
		const FTaskNodeResuleHelper& TaskNodeResuleHelper
		);

	FTaskNodeResuleHelper ConsumeEvent(
		const FGuid& InGuid
		);

	virtual FString GetGuideThreadTitle() const;

	// 任务节点类型：支线/支线
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag TaskNodeCategory;

	// 任务描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	/**
	 * 任务完成后的奖励
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, int32> RewardProxysMap;

	FOnGuideThreadNameChagned OnGuideThreadNameChagned;

protected:
	// 任务名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TaskName;

	/**
	 * 引导任务内，某些选项的结果保存在这里
	 */
	TMap<FGuid, FTaskNodeResuleHelper> EventsSet;

	FTaskNodeDescript CurrentTaskNodeDescript;

private:
	bool bIsComleted = false;
};

UCLASS()
class UStateTreeGuideThreadComponentSchema : public UStateTreeComponentSchema
{
	GENERATED_BODY()

public:
	using FOwnerType = AGuideThread;

	UStateTreeGuideThreadComponentSchema();

	virtual bool IsStructAllowed(
		const UScriptStruct* InScriptStruct
		) const override;

	virtual void PostLoad() override;

	static bool SetContextRequirements(
		UBrainComponent& BrainComponent,
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		);

	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<APlanetPlayerController> PlayerControllerClass = nullptr;
};

/**
 *
 */
UCLASS()
class PLANET_API UGuideThreadSystemStateTreeComponent : public UGuideSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	virtual bool SetContextRequirements(
		FStateTreeExecutionContext& Context,
		bool bLogErrors = false
		) override;
};
#pragma endregion

#pragma region 主线
/**
 *	主线任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideThread_Main : public AGuideThread,
                                     public IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	AGuideThread_Main(
		const FObjectInitializer& ObjectInitializer
		);

	virtual EGuideThreadType GetGuideThreadType() const override final;
};

UCLASS()
class UStateTreeGuideMainThreadComponentSchema : public UStateTreeGuideThreadComponentSchema
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
class PLANET_API UGuideMainThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponent
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
class PLANET_API AGuideThread_Branch : public AGuideThread,
                                       public IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	virtual EGuideThreadType GetGuideThreadType() const override final;
};
#pragma endregion

#pragma region 即时任务
/**
 *	即时事件，如即时挑战
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideThread_Immediate : public AGuideThread
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
class PLANET_API AGuideThread_Area : public AGuideThread_Immediate,
                                     public IGuideThreadInterfacetion
{
	GENERATED_BODY()

public:
	AGuideThread_Area(
		const FObjectInitializer& ObjectInitializer
		);

	virtual EGuideThreadType GetGuideThreadType() const override final;
};

UCLASS()
class UStateTreeGuideAreaThreadComponentSchema : public UStateTreeGuideThreadComponentSchema
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
class PLANET_API UGuideAreaThreadSystemStateTreeComponent : public UGuideThreadSystemStateTreeComponent
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
