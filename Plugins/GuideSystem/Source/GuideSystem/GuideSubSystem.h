// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GuideSubSystem.generated.h"

class AGuideActorBase;
class AGuideThreadBase;
class AGuideThread_MainBase;
class AGuideThread_BranchBase;
class AGuideThread_ImmediateBase;

using FOnStartGuide = TMulticastDelegate<void(
	AGuideThreadBase*
)>;

using FOnStopGuide = TMulticastDelegate<void(
	AGuideThreadBase*
)>;

struct FTaskNodeDescript;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGetGuideSubSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class GUIDESYSTEM_API IGetGuideSubSystemInterface
{
	GENERATED_BODY()

public:
	virtual UGuideSubSystem* GetGuideSubSystem()const = 0;
};

/**
 *	引导/任务系统
 *	包含：主线、支线、新手引导之类
 */
UCLASS()
class GUIDESYSTEM_API UGuideSubSystem : public UWorldSubsystem
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
		const TSubclassOf<AGuideThread_BranchBase>& BranchLineGuideClass
	);

	/**
	 * 将任务设置为当前任务
	 * 只能设置主线（AGuideThread_MainBase）或者支线（AGuideThread_BranchBase）
	 * @param GuideClass 
	 */
	void ActiveBrandGuideThread(
		const TSubclassOf<AGuideThread_BranchBase>& GuideClass
	);

	/**
	 * 停止当前任务，如果类型为指定类型的话
	 * @param GuideClass 
	 * @param bIsTransit 
	 */
	void StopActiveBrandGuideThread(
		const TSubclassOf<AGuideThread_BranchBase>& GuideClass
	);

	/**
	 * 开始并行引导
	 * 只能是 AGuideThread_ImmediateEvents 或 AGuideThread_Area
	 */
	void StartParallelGuideThread(
		const TSubclassOf<AGuideThread_ImmediateBase>& GuideClass
	);

	void StopParallelGuideThread(
		const TSubclassOf<AGuideThread_ImmediateBase>& GuideClass
	);

	void OnGuideThreadStoped(
		AGuideThreadBase* GuideThreadPtr
	);

	/**
	 * @return 当前激活的任务 
	 */
	TObjectPtr<AGuideThreadBase> GetCurrentGuideThread() const;
	
	/**
	 * @return 当前的主线任务
	 */
	TSubclassOf<AGuideThread_MainBase> GetCurrentMainGuideThread()const;
	
	/**
	 * @return 所有拥有的支线任务
	 */
	TArray<TSubclassOf<AGuideThread_BranchBase>> GetGuideThreadAry()const;

	TObjectPtr<AGuideThreadBase> IsActivedGuideThread(const TSubclassOf<AGuideThread_ImmediateBase>& GuideClass)const;
	
	FOnStartGuide& GetOnStartGuide();

	FOnStopGuide& GetOnStopGuide();

	/**
	 * 添加引导已执行到的子节点的记录
	 * @param GuideID 
	 * @param TaskId 
	 */
	void AddGuidePostion(const FGuid &CurrentGuideID, const FGuid& CurrentTaskID);
	
	/**
	 * 消耗引导已执行到的子节点的记录
	 * @param GuideID 
	 * @return 
	 */
	bool ConsumeGuidePostion(const FGuid &CurrentGuideID, FGuid& CurrentTaskID);
	
	void RemoveGuidePostion(const FGuid &CurrentGuideID);
	
private:
	virtual TArray<TSubclassOf<AGuideThread_MainBase>> GetMainGuideThreadChaptersAry()const;
	
	virtual TSubclassOf<AGuideThread_MainBase> GetToBeContinueGuideThread()const;
	
	void ActiveTargetGuideThread(
		const TSubclassOf<AGuideThreadBase>& GuideClass
	);

	FOnStartGuide OnStartGuide;

	FOnStopGuide OnStopGuide;

	// 上一个激活的引导任务
	TSubclassOf<AGuideThreadBase> PreviouGuideClass;

	/**
	 * 当前追踪的引导
	 * 只能是主线或者支线
	 * 同一只时间只能激活一个引导
	 */
	TObjectPtr<AGuideThreadBase> CurrentLineGuidePtr = nullptr;

	/**
	 * 当前正在追踪的引导
	 */
	TArray<TObjectPtr<AGuideThreadBase>> ActivedGuideThreadsAry;

	/**
	 * 可追踪的支线任务
	 */
	TArray<TSubclassOf<AGuideThread_BranchBase>> GuideThreadBranchAry;

	/**
	 * Key		引导任务ID
	 * Value	已执行到的任务ID
	 */
	TMap<FGuid, FGuid> GuidePostionSet;

	/**
	 * 当前的主线引导章节
	 */
	uint8 CurrentMainGuideThreadIndex = 0;
};
