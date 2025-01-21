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

using FOnCurrentTaskNodeChanged = TMulticastDelegate<void(const TSoftObjectPtr<UPAD_TaskNode_Guide>&)>;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

/**
 *	与NPC对话时的系列任务
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideInteractionActor : public AGuideMainThread
{
	GENERATED_BODY()

public:
	AGuideInteractionActor(const FObjectInitializer& ObjectInitializer);

	// 玩家Character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

	// 这个引导所属的Character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AHumanCharacter* TargetCharacter = nullptr;

	FOnGuideInteractionEnd OnGuideInteractionEnd;
};
