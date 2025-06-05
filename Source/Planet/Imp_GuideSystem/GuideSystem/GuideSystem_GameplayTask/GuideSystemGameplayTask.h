// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "BehaviorTree/Tasks/BTTask_RunDynamicStateTree.h"

#include "ProxyProcessComponent.h"
#include "LayoutCommon.h"
#include "MainMenuCommon.h"
#include "GameplayTask_GuideTask_Base.h"

#include "GuideSystemGameplayTask.generated.h"

class APlanetPlayerController;
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
class PLANET_API UGameplayTask_Base : public UGameplayTask_GuideTask_Base
{
	GENERATED_BODY()

public:
	void SetPlayerCharacter(
		AHumanCharacter_Player* PlayerCharacterPtr
		);

private:

protected:
	TObjectPtr<AHumanCharacter_Player> PlayerCharacterPtr = nullptr;
};

UCLASS()
class PLANET_API UGameplayTask_WaitInteractionSceneActor : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	UGameplayTask_WaitInteractionSceneActor(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	TSoftObjectPtr<UPAD_GuideThread_WaitInteractionSceneActor> PAD = nullptr;

	TSubclassOf<ATargetPoint_Runtime>TargetPoint_RuntimeClass;

protected:
	void OnInteractionSceneActor(
		ISceneActorInteractionInterface* TargetActorPtr
		);

	FDelegateHandle DelegateHandle;

private:
	
	ATargetPoint_Runtime* TargetPointPtr = nullptr;

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

	UGameplayTask_WaitPlayerEquipment(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
		) override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	bool bPlayerAssign = true;

	FGameplayTag WeaponSocket;

	FGameplayTag SkillSocket;

	bool bIsEquipentCharacter = false;

protected:
	UFUNCTION()
	void OnCharacterSocketUpdated(
		const FCharacterSocket& Socket,
		const FGameplayTag& ProxyType
		);

	void OnMembersChanged(
		const FTeammate& Teammate,
		const TSharedPtr<FCharacterProxy>& CharacterProxySPtr
		);

	bool bIsComplete = false;

	FDelegateHandle DelegateHandle;

	FMemberChangedDelegate MemberChangedDelegate;
};

UCLASS()
class PLANET_API UGameplayTask_WaitOpenLayout : public UGameplayTask_Base
{
	GENERATED_BODY()

public:
	using FOnSwitchToLayout =
	TCallbackHandleContainer<void(ELayoutCommon)>::FCallbackHandleSPtr;

	using FOnSwitchToMenuLayout =
	TCallbackHandleContainer<void(EMenuType)>::FCallbackHandleSPtr;

	UGameplayTask_WaitOpenLayout(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void Activate() override;

	virtual void OnDestroy(
		bool bInOwnerFinished
		) override;

	ELayoutCommon TargetLayoutCommon = ELayoutCommon::kMenuLayout;
	
	EMenuType TargetMenuType = EMenuType::kAllocationSkill;
	
	TObjectPtr<APlanetPlayerController> PCPtr = nullptr;

protected:

	void OnSwitchToNewLayout(ELayoutCommon Layout);
	
	void OnSwitchToNewMenuLayout(EMenuType MenuType);
	
	FOnSwitchToLayout OnSwitchToLayout;
	
	FOnSwitchToMenuLayout OnSwitchToMenuLayout;
};
