
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GuideSubSystem.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;

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
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	static UGuideSubSystem* GetInstance();

	// 初始化主线内容
	void InitializeMainThread();

	void ActiveMainThread();
	
	// 添加支线内容
	void AddBranchLine(const TSubclassOf<AGuideThread_Branch>& BranchLineGuideClass);

	// 将任务设置为当前任务
	void ActiveTargetGuideThread(const TSubclassOf<AGuideThread>& GuideClass, bool bIsTransit);
	
	// 停止当前任务，如果类型为指定类型的话
	void StopActiveTargetGuideThread(const TSubclassOf<AGuideThread>& GuideClass);

	void GuideThreadEnded(AGuideThread * GuideThreadPtr);
	
	TObjectPtr<AGuideThread>GetCurrentGuideThread()const;

	FOnGuideEnd & GetOnGuideEnd();
	
	FOnCurrentGuideChagned OnCurrentGuideChagned;

private:

	// TODO 选中其他任务
	FOnGuideEnd OnGuideEnd;
	
	// 上一个激活的引导任务
	TSubclassOf<AGuideThread> PreviouGuideClass;
	
	/**
	 * 当前追踪的引导
	 * 同一只时间只能激活一个引导
	 */
	TObjectPtr<AGuideThread> CurrentLineGuidePtr = nullptr;

	// 引导任务ID, 已执行到的任务ID
	TMap<FGuid, FGuid> GuidePostionSet;

};
