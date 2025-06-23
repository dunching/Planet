// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "QuestSubSystem.generated.h"

class AQuestActorBase;
class AQuestChainBase;
class AQuestChain_MainBase;
class AQuestChain_BranchBase;
class AQuestChain_ImmediateBase;

using FOnStartGuide = TMulticastDelegate<void(
	AQuestChainBase*
)>;

using FOnStopGuide = TMulticastDelegate<void(
	AQuestChainBase*
)>;

struct FTaskNodeDescript;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UGetQuestSubSystemInterface : public UInterface
{
	GENERATED_BODY()
};

class QUESTSYSTEM_API IGetQuestSubSystemInterface
{
	GENERATED_BODY()

public:
	virtual UQuestSubSystem* GetGuideSubSystem()const = 0;
};

/**
 *	引导/任务系统
 *	包含：主线、支线、新手引导之类
 */
UCLASS()
class QUESTSYSTEM_API UQuestSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(
		UObject* Outer
	) const override;

	static UQuestSubSystem* GetInstance();

	// 初始化主线内容
	void InitializeMainThread();

	void ActiveMainThread();

	// 添加支线内容
	void AddGuideThread_Brand(
		const TSubclassOf<AQuestChain_BranchBase>& BranchLineGuideClass
	);

	/**
	 * 将任务设置为当前任务
	 * 只能设置主线（AGuideThread_MainBase）或者支线（AGuideThread_BranchBase）
	 * @param GuideClass 
	 */
	void ActiveBrandGuideThread(
		const TSubclassOf<AQuestChain_BranchBase>& GuideClass
	);

	/**
	 * 停止当前任务，如果类型为指定类型的话
	 * @param GuideClass 
	 * @param bIsTransit 
	 */
	void StopActiveBrandGuideThread(
		const TSubclassOf<AQuestChain_BranchBase>& GuideClass
	);

	/**
	 * 开始并行引导
	 * 只能是 AGuideThread_ImmediateEvents 或 AGuideThread_Area
	 */
	void StartParallelGuideThread(
		const TSubclassOf<AQuestChain_ImmediateBase>& GuideClass
	);

	void StopParallelGuideThread(
		const TSubclassOf<AQuestChain_ImmediateBase>& GuideClass
	);

	void OnGuideThreadStoped(
		AQuestChainBase* GuideThreadPtr
	);

	/**
	 * @return 当前激活的任务 
	 */
	TObjectPtr<AQuestChainBase> GetCurrentGuideThread() const;
	
	/**
	 * @return 当前的主线任务
	 */
	TSubclassOf<AQuestChain_MainBase> GetCurrentMainGuideThread()const;
	
	/**
	 * @return 所有拥有的支线任务
	 */
	TArray<TSubclassOf<AQuestChain_BranchBase>> GetGuideThreadAry()const;

	/**
	 * 查询该类型的任务是否被激活，如查询是否处于挑战模式
	 * TODO：改成Tag以用于包含子集
	 * @param GuideClass 
	 * @return 
	 */
	TObjectPtr<AQuestChainBase> IsActivedGuideThread(const TSubclassOf<AQuestChain_ImmediateBase>& GuideClass)const;

	const TArray<TObjectPtr<AQuestChainBase>> &GetActivedGuideThreadsAry()const;
	
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
	virtual TArray<TSubclassOf<AQuestChain_MainBase>> GetMainGuideThreadChaptersAry()const;
	
	virtual TSubclassOf<AQuestChain_MainBase> GetToBeContinueGuideThread()const;
	
	void ActiveTargetGuideThread(
		const TSubclassOf<AQuestChainBase>& GuideClass
	);

	FOnStartGuide OnStartGuide;

	FOnStopGuide OnStopGuide;

	// 上一个激活的引导任务
	TSubclassOf<AQuestChainBase> PreviouGuideClass;

	/**
	 * 当前追踪的引导
	 * 只能是主线或者支线
	 * 同一只时间只能激活一个引导
	 */
	TObjectPtr<AQuestChainBase> CurrentLineGuidePtr = nullptr;

	/**
	 * 当前正在追踪的引导
	 */
	TArray<TObjectPtr<AQuestChainBase>> ActivedGuideThreadsAry;

	/**
	 * 可追踪的支线任务
	 */
	TArray<TSubclassOf<AQuestChain_BranchBase>> GuideThreadBranchAry;

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
