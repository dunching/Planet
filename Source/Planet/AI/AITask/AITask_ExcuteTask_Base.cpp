#include "AITask_ExcuteTask_Base.h"
// Copyright Epic Games, Inc. All Rights Reserved.

#include "UObject/Package.h"
#include "TimerManager.h"
#include "AISystem.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"
#include "AIResources.h"
#include "GameplayTasksComponent.h"

#include "ProxyProcessComponent.h"

#include "CharacterBase.h"
#include "Skill_Base.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"

#include "AIComponent.h"
#include "AITask_ReleaseSkill.h"

UAITask_ExcuteTask_Base::UAITask_ExcuteTask_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;
}

void UAITask_ExcuteTask_Base::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_ExcuteTask_Base::ConditionalPerformTask()
{
}

void UAITask_ExcuteTask_Base::SetUp(UPAD_TaskNode_Preset* InCurrentTaskNodePtr, AHumanCharacter_AI* InCharacterPtr)
{
	CurrentTaskNodePtr = InCurrentTaskNodePtr;
	CharacterPtr = InCharacterPtr;
}

void UAITask_ExcuteTask_Base::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

UAITask_ExcuteTemporaryTask_Base::UAITask_ExcuteTemporaryTask_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsPausable = true;
}

void UAITask_ExcuteTemporaryTask_Base::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UAITask_ExcuteTemporaryTask_Base::ConditionalPerformTask()
{
}

void UAITask_ExcuteTemporaryTask_Base::SetUp(UTaskNode_Temporary* InCurrentTaskNodePtr, AHumanCharacter_AI* InCharacterPtr)
{
	CurrentTaskNodePtr = InCurrentTaskNodePtr;
	CharacterPtr = InCharacterPtr;
}

void UAITask_ExcuteTemporaryTask_Base::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
