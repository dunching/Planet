
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GuideSubSystem.generated.h"

class AGuideActor;

using FOnCurrentGuideChagned = TMulticastDelegate<void(AGuideActor*)>;

using FOnGuideEnd = TMulticastDelegate<void(AGuideActor*)>;

/**
 *	引导/任务系统
 *	包含：主线、支线、新手引导之类
 */
UCLASS()
class PLANET_API UGuideSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UGuideSubSystem* GetInstance();

	// 初始化主线内容
	void InitializeMainLine();

	void ActiveMainLine();
	
	// 添加支线内容
	void AddBranchLine(const TSubclassOf<AGuideActor>& BranchLineGuideClass);

	FOnCurrentGuideChagned OnCurrentGuideChagned;

	// TODO 选中其他任务
	FOnGuideEnd OnGuideEnd;
	
	// 当前追踪的引导
	TObjectPtr<AGuideActor> CurrentLineGuidePtr = nullptr;

private:

	// UPROPERTY(ReplicatedUsing = OnRep_WolrdProcess)
	TObjectPtr<AGuideActor> MainLineGuidePtr = nullptr;

	TArray<TObjectPtr<AGuideActor>> BranchLineAry;
};
