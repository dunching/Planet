// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/AITask.h"
#include "ProxyProcessComponent.h"

#include "BehaviorTree/Tasks/BTTask_RunDynamicStateTree.h"

#include "GuideSystemGameplayTask.generated.h"

class AHumanCharacter_Player;
class ATargetPoint_Runtime;
class ASceneActor;
class AGuideThread;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;
class UPAD_GuideThread_WaitInteractionSceneActor;

UCLASS()
class PLANET_API UGameplayTask_Base : public UGameplayTask
{
	GENERATED_BODY()

public:
	
	void SetPlayerCharacter(AHumanCharacter_Player* PlayerCharacterPtr);

	void SetTaskID(const FGuid& InTaskID);

	EStateTreeRunStatus GetStateTreeRunStatus()const;
	
protected:

	EStateTreeRunStatus StateTreeRunStatus = EStateTreeRunStatus::Running;
	
	AHumanCharacter_Player* PlayerCharacterPtr = nullptr;

	FGuid TaskID;

};

UCLASS()
class PLANET_API UGameplayTask_WaitInteractionSceneActor : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	
	UGameplayTask_WaitInteractionSceneActor(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	TSoftObjectPtr<UPAD_GuideThread_WaitInteractionSceneActor> PAD = nullptr;

protected:

	UFUNCTION()
	void OnInteractionSceneActor(ASceneActor* TargetActorPtr);

	bool bIsInteractionSceneActor = false;

	FDelegateHandle DelegateHandle;
	
};

UCLASS()
class PLANET_API UGameplayTask_WaitPlayerEquipment : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	
	UGameplayTask_WaitPlayerEquipment(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	FGameplayTag WeaponSocket;

	FGameplayTag SkillSocket;

protected:

	UFUNCTION()
	void OnCharacterSocketUpdated(const FCharacterSocket&Socket);

	bool bIsComplete = false;

	FDelegateHandle DelegateHandle;
	
};
