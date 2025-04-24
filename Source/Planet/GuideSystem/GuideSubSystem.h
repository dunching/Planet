// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GuideSubSystem.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;
class AGuideThread_Immediate;

using FOnStartGuide = TMulticastDelegate<void(
	AGuideThread*
)>;

using FOnStopGuide = TMulticastDelegate<void(
	AGuideThread*
)>;

struct FTaskNodeDescript;

/**
 *	引导/任务系统
 *	包含：主线、支线、新手引导之类
 */
UCLASS()
class PLANET_API UGuideSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(
		UObject* Outer
	) const override;

	static UGuideSubSystem* GetInstance();

	// 初始化主线内容
	void InitializeMainThread();

	void ActiveMainThread();

	// 添加支线内容
	void AddGuideThread_Brand(
		const TSubclassOf<AGuideThread_Branch>& BranchLineGuideClass
	);

	/**
	 * 将任务设置为当前任务
	 * 只能设置主线（AGuideThread_Main）或者支线（AGuideThread_Branch）
	 * @param GuideClass 
	 */
	void ActiveBrandGuideThread(
		const TSubclassOf<AGuideThread_Branch>& GuideClass
	);

	/**
	 * 停止当前任务，如果类型为指定类型的话
	 * @param GuideClass 
	 * @param bIsTransit 
	 */
	void StopActiveBrandGuideThread(
		const TSubclassOf<AGuideThread_Branch>& GuideClass
	);

	/**
	 * 开始并行引导
	 * 只能是 AGuideThread_ImmediateEvents 或 AGuideThread_Area
	 */
	void StartParallelGuideThread(
		const TSubclassOf<AGuideThread_Immediate>& GuideClass
	);

	void StopParallelGuideThread(
		const TSubclassOf<AGuideThread_Immediate>& GuideClass
	);

	void GuideThreadEnded(
		AGuideThread* GuideThreadPtr
	);

	TObjectPtr<AGuideThread> GetCurrentGuideThread() const;

	FOnStartGuide& GetOnStartGuide();

	FOnStopGuide& GetOnStopGuide();

private:
	void ActiveTargetGuideThread(
		const TSubclassOf<AGuideThread>& GuideClass
	);

	FOnStartGuide OnStartGuide;

	FOnStopGuide OnStopGuide;

	// 上一个激活的引导任务
	TSubclassOf<AGuideThread> PreviouGuideClass;

	/**
	 * 当前追踪的引导
	 * 只能是主线或者支线
	 * 同一只时间只能激活一个引导
	 */
	TObjectPtr<AGuideThread> CurrentLineGuidePtr = nullptr;

	/**
	 * 当前正在追踪的引导
	 */
	TArray<TObjectPtr<AGuideThread>> ActivedGuideThreadsAry;

	/**
	 * 可追踪的支线任务
	 */
	TArray<TSubclassOf<AGuideThread_Branch>> GuideThreadBranchAry;

	/**
	 * Key		引导任务ID
	 * Value	已执行到的任务ID
	 */
	TMap<FGuid, FGuid> GuidePostionSet;
};
