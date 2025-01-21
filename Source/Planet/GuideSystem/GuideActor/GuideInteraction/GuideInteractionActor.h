// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"
#include "GuideThreadActor.h"

#include "GuideInteractionActor.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

UCLASS()
class UStateTreeGuideInteractionComponentSchema : public UStateTreeComponentSchema
{
	GENERATED_BODY()
public:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;

};

/**
 *
 */
UCLASS()
class PLANET_API UGuideInteractionSystemStateTreeComponent : public UGuideSystemStateTreeComponent
{
	GENERATED_BODY()

public:
	virtual TSubclassOf<UStateTreeSchema>GetSchema() const override;
};

/**
 *	与NPC对话时的系列任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideInteractionActor : public AGuideActor
{
	GENERATED_BODY()

public:
	AGuideInteractionActor(const FObjectInitializer& ObjectInitializer);

	// 玩家Character
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Content)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

	// 这个引导所属的Character
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Content)
	AHumanCharacter* TargetCharacter = nullptr;

	// 交互节点名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString InteractionNodeName;

	FOnGuideInteractionEnd OnGuideInteractionEnd;
};
