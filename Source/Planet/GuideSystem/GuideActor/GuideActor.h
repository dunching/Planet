// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"

#include "GuideActor.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

using FOnCurrentTaskNodeChanged = TMulticastDelegate<void(const TSoftObjectPtr<UPAD_TaskNode_Guide>&)>;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

/**
 *	引导,执行链条或拓扑结构的系列任务
 *	包含：主线、支线、新手引导之类
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API AGuideActor : public AInfo
{
	GENERATED_BODY()

public:
	AGuideActor(const FObjectInitializer& ObjectInitializer);

	UGameplayTasksComponent* GetGameplayTasksComponent() const;

	UGuideSystemStateTreeComponent* GetGuideSystemStateTreeComponent() const;

	FOnCurrentTaskNodeChanged OnCurrentTaskNodeChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGuideSystemStateTreeComponent> GuideStateTreeComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;
};
