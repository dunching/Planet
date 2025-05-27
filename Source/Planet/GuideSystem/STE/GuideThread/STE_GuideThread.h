// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateTypes.h"
#include "STE_Base.h"

#include "STE_GuideThread.generated.h"

class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;
class AGuideThread_Area;
class AGuideThread_Challenge;
class AGuideInteraction_Actor;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

#pragma region Base
UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideThread : public UObject
{
	GENERATED_BODY()

public:

	FGuid LastTaskID;
	
};

UCLASS(Blueprintable)
class PLANET_API USTE_GuideThread : public USTE_Base
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = Output)
	AHumanCharacter_Player* PlayerCharacter = nullptr;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideThread* GuideThreadActorPtr = nullptr;

private:
	
	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output, meta=(AllowPrivateAccess="true"))
	UGloabVariable_GuideThread* GloabVariable = nullptr;

};
#pragma endregion 

#pragma region 主线
UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideThread_Main : public UObject
{
	GENERATED_BODY()

public:

	TArray<TWeakObjectPtr<ACharacterBase>> SpwanedCharacterAry;

	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;

};

UCLASS(Blueprintable)
class PLANET_API USTE_GuideThread_Main : public USTE_GuideThread
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual auto TreeStop(FStateTreeExecutionContext& Context) -> void override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideThread_Main* GuideThreadMainActorPtr = nullptr;

private:
	
	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output, meta=(AllowPrivateAccess="true"))
	UGloabVariable_GuideThread_Main* GloabVariable_MainPtr = nullptr;

};
#pragma endregion 

#pragma region 支线
UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideThread_Brand : public UObject
{
	GENERATED_BODY()

public:

	TArray<TWeakObjectPtr<ACharacterBase>> SpwanedCharacterAry;

	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;

};

UCLASS(Blueprintable)
class PLANET_API USTE_GuideThread_Brand : public USTE_GuideThread
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideThread_Branch* GuideThreadBranchActorPtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_GuideThread_Brand* GloabVariable_Brand = nullptr;

};
#pragma endregion

#pragma region 区域
UCLASS(Blueprintable)
class PLANET_API UGloabVariable_GuideThread_Area : public UObject
{
	GENERATED_BODY()

public:

	TArray<TWeakObjectPtr<ACharacterBase>> SpwanedCharacterAry;

	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;

};

UCLASS(Blueprintable)
class PLANET_API USTE_GuideThread_Area : public USTE_GuideThread
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual auto TreeStop(FStateTreeExecutionContext& Context) -> void override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideThread_Area* GuideThreadAreaActorPtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_GuideThread_Area* GloabVariable_Area = nullptr;

};
#pragma endregion
