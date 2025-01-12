// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "ProxyProcessComponent.h"
#include "TaskNode.h"

#include "AITask_ExcuteTask_Base.generated.h"


struct FSocket_FASI;

class UGameplayAbility;

class AHumanCharacter_AI;

class UPAD_TaskNode;
class UPAD_TaskNode_Preset;
class UTaskNode_Temporary;
class UAIComponent;

UCLASS()
class PLANET_API UAITask_ExcuteTask_Base : public UAITask
{
	GENERATED_BODY()

public:
	UAITask_ExcuteTask_Base(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

	void SetUp(UPAD_TaskNode_Preset* CurrentTaskNodePtr, AHumanCharacter_AI* InCharacterPtr);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bOwnerFinished) override;

	AHumanCharacter_AI* CharacterPtr = nullptr;

	UPROPERTY(Transient)
	UPAD_TaskNode_Preset* CurrentTaskNodePtr = nullptr;
};

UCLASS()
class PLANET_API UAITask_ExcuteTemporaryTask_Base : public UAITask
{
	GENERATED_BODY()

public:
	UAITask_ExcuteTemporaryTask_Base(const FObjectInitializer& ObjectInitializer);

	void ConditionalPerformTask();

	void SetUp(UTaskNode_Temporary* CurrentTaskNodePtr, AHumanCharacter_AI* InCharacterPtr);

protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool bOwnerFinished) override;

	AHumanCharacter_AI* CharacterPtr = nullptr;

	UPROPERTY(Transient)
	UTaskNode_Temporary* CurrentTaskNodePtr = nullptr;
};
