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

USTRUCT(BlueprintType, Blueprintable)
struct FTaskNodeResuleHelper
{
	GENERATED_USTRUCT_BODY()

	bool GetIsValid() const;

	// 任务ID
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid TaskID;

	// 执行结果 (-1表示无效)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Output_1 = -1;
};

struct FTaskNodeDescript
{
	bool GetIsValid() const;
	
	// 执行此节点时，是否刷新上一条的描述？
	// 在执行某些H节点时，是不需要刷新的
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFreshPreviouDescription = true;
	
	FString Name;
	
	FString Description;
};

using FOnCurrentTaskNodeChanged = TMulticastDelegate<void(const FTaskNodeDescript&)>;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GuideST)
	TObjectPtr<UGuideSystemStateTreeComponent> GuideStateTreeComponentPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;
};
