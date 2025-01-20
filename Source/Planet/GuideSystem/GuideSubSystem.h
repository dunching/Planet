
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GuideSubSystem.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideMainThread;
class AGuideBranchThread;

using FOnCurrentGuideChagned = TMulticastDelegate<void(AGuideThread*)>;

using FOnGuideEnd = TMulticastDelegate<void(AGuideThread*)>;

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
	void AddBranchLine(const TSubclassOf<AGuideBranchThread>& BranchLineGuideClass);

	TObjectPtr<AGuideThread>GetCurrentGuideThread()const;
	
	FOnCurrentGuideChagned OnCurrentGuideChagned;

	// TODO 选中其他任务
	FOnGuideEnd OnGuideEnd;
	
private:

	// 当前追踪的引导
	TObjectPtr<AGuideThread> CurrentLineGuidePtr = nullptr;

	// UPROPERTY(ReplicatedUsing = OnRep_WolrdProcess)
	TObjectPtr<AGuideMainThread> MainLineGuidePtr = nullptr;

	TArray<TObjectPtr<AGuideBranchThread>> BranchLineAry;
};
