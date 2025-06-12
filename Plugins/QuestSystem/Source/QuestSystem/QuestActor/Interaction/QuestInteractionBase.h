// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "Components/StateTreeComponentSchema.h"

#include "QuestSystemStateTreeComponent.h"
#include "QuestsActorBase.h"

#include "QuestInteractionBase.generated.h"


class UGameplayTasksComponent;

class UQuestSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacter;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

#pragma region Base

/**
 * 交互类的任务
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEM_API AQuestInteractionBase : public AQuestActorBase
{
	GENERATED_BODY()

public:
	AQuestInteractionBase(const FObjectInitializer& ObjectInitializer);

	// 交互节点名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InteractionNodeName;

	/**
	 * 提前终止交互
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWantToStop = false;
	
};

UCLASS()
class QUESTSYSTEM_API USTQuestInteractionComponentSchemaBase : public UStateTreeComponentSchema
{
	GENERATED_BODY()
public:
	using FOwnerType = AQuestInteractionBase;

	USTQuestInteractionComponentSchemaBase();
	
	virtual void PostLoad() override;
	
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

	static bool SetContextRequirements(UBrainComponent& BrainComponent, FStateTreeExecutionContext& Context, bool bLogErrors = false);

	/**
	 * 交互的本体
	 * 如玩家跟门交互，此类型则为玩家
	 */
	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<ACharacter> ACharacterClass = nullptr;
	
	/**
	 * 交互的目标的类型
	 * 如玩家跟门交互，此类型则为门
	 */
	UPROPERTY(EditAnywhere, Category = "Defaults", NoClear)
	TSubclassOf<AActor> InteractionTargetClass = nullptr;
	
};

/**
 *
 */
UCLASS()
class QUESTSYSTEM_API UQuestInteractionSystemSTComponentBase : public UQuestSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	using FSchemaType = USTQuestInteractionComponentSchemaBase;
	
	virtual TSubclassOf<UStateTreeSchema>GetSchema() const override;
	
	virtual bool SetContextRequirements(FStateTreeExecutionContext& Context, bool bLogErrors = false) override;
};
#pragma endregion
