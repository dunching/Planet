// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GuideSystem_GenericType.h"

#include "GuideActorBase.generated.h"


class UGameplayTasksComponent;

class UGuideSystemStateTreeComponent;
class UPAD_TaskNode_Guide;
class AGuideThread;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

using FOnGuideInteractionEnd = TMulticastDelegate<void()>;

/**
 *	引导,执行链条或拓扑结构的系列任务
 *	包含：主线、支线、新手引导之类
 */
UCLASS(BlueprintType, Blueprintable)
class GUIDESYSTEM_API AGuideActorBase : public AInfo
{
	GENERATED_BODY()

public:
	
	using FOnCurrentTaskNodeChanged = TMulticastDelegate<void(const FTaskNodeDescript&)>;

	AGuideActorBase(const FObjectInitializer& ObjectInitializer);

	virtual void Destroyed() override;

	void ActiveGuide();
	
	UGameplayTasksComponent* GetGameplayTasksComponent() const;

	UGuideSystemStateTreeComponent* GetGuideSystemStateTreeComponent() const;

	FGuid GetCurrentTaskID() const;

	/**
	 * 记录当前执行到的任务的ID
	 * @param PreviousGuideID 
	 */
	void SetCurrentTaskID(
		const FGuid& TaskID
		);

	FGuid GetPreviousTaskID() const;

	/**
	 * 设置上一次执行到的任务的ID
	 * @param PreviousGuideID 
	 */
	void SetPreviousTaskID(
		const FGuid& PreviousGuideID
		);

	FGuid GetGuideID() const;

	FOnCurrentTaskNodeChanged OnCurrentTaskNodeChanged;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tags)
	FGameplayTagContainer ActivedTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GuideST)
	TObjectPtr<UGuideSystemStateTreeComponent> GuideStateTreeComponentPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;

private:
	/**
	 * 这条引导的ID
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta=(AllowPrivateAccess="true"))
	FGuid GuideID;

	/**
	 * 这个引导任务当前执行到的任务的ID
	 */
	FGuid CurrentTaskID;

	/**
	 * 这个引导任务上次执行到的任务的ID
	 */
	FGuid PreviousTaskID;

};
