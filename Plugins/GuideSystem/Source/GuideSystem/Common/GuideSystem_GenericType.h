// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BrainComponent.h"
#include "GameplayTagContainer.h"

#include "GuideSystem_GenericType.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class AGuideThread;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

USTRUCT(BlueprintType, Blueprintable)
struct GUIDESYSTEM_API FTaskNodeResuleHelper
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

USTRUCT(BlueprintType, Blueprintable)
struct GUIDESYSTEM_API FTaskNodeDescript
{
	GENERATED_USTRUCT_BODY()

	FTaskNodeDescript();
	
	FTaskNodeDescript(bool bIsOnlyFresh);
	
	static FTaskNodeDescript Refresh; 
	
	bool GetIsValid() const;
	
	// 执行此节点时，是否刷新上一条的描述？
	// 在执行某些H节点时，是不需要刷新的
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFreshPreviouDescription = true;
	
	/**
	 * 是否强制刷新
	 */
	bool bIsOnlyFresh = false;
	
	FString Name;
	
	FString Description;

	FGuid StepTaskID;
};
