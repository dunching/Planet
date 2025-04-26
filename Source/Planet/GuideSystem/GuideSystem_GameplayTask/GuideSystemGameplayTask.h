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
class ISceneActorInteractionInterface;
class ASceneActor;
class AGuideThread;
class UPAD_TaskNode_Guide_AddToTarget;
class UPAD_TaskNode_Guide_ConversationWithTarget;
class UPAD_TaskNode_Interaction_Option;
class UPAD_TaskNode_Interaction_NotifyGuideThread;
class UPAD_GuideThread_WaitInteractionSceneActor;

struct FTeammate;
struct FCharacterProxy;

UCLASS()
class PLANET_API UGameplayTask_Base : public UGameplayTask
{
	GENERATED_BODY()

public:
	
	EStateTreeRunStatus GetStateTreeRunStatus()const;

	void SetPlayerCharacter(AHumanCharacter_Player* PlayerCharacterPtr);

private:
	
	void SetTaskID(const FGuid& InTaskID);

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
	
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	TSoftObjectPtr<UPAD_GuideThread_WaitInteractionSceneActor> PAD = nullptr;

protected:

	void OnInteractionSceneActor(ISceneActorInteractionInterface* TargetActorPtr);

	FDelegateHandle DelegateHandle;
	
};

UCLASS()
class PLANET_API UGameplayTask_WaitPlayerEquipment : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	
	using FMemberChangedDelegate =
	TCallbackHandleContainer<void(
		const FTeammate&,
		const TSharedPtr<FCharacterProxy>&
	)>::FCallbackHandleSPtr;

	UGameplayTask_WaitPlayerEquipment(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime)override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	bool bPlayerAssign = true;
	
	FGameplayTag WeaponSocket;

	FGameplayTag SkillSocket;

	bool bIsEquipentCharacter = false;
	
protected:

	UFUNCTION()
	void OnCharacterSocketUpdated(const FCharacterSocket&Socket,const FGameplayTag&ProxyType);

	void OnMembersChanged(
		const FTeammate&Teammate,
		const TSharedPtr<FCharacterProxy>&CharacterProxySPtr
		);

	bool bIsComplete = false;

	FDelegateHandle DelegateHandle;

	FMemberChangedDelegate MemberChangedDelegate;
};
