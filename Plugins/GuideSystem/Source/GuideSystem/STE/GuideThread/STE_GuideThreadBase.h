// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Templates/SubclassOf.h"
#include <Perception/AIPerceptionTypes.h>
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"

#include "STE_Base.h"

#include "STE_GuideThreadBase.generated.h"

class AGuideThread;
class AGuideThreadBase;
class AGuideThread_MainBase;
class AGuideThread_BranchBase;
class AGuideThread_AreaBase;
class AGuideThread_ChallengeBase;
class AGuideInteractionBase;

#pragma region Base
UCLASS(Blueprintable)
class GUIDESYSTEM_API UGloabVariable_GuideThreadBase : public UObject
{
	GENERATED_BODY()

public:
	FGuid LastTaskID;
};

UCLASS(Blueprintable)
class GUIDESYSTEM_API USTE_GuideThreadBase : public USTE_Base
{
	GENERATED_BODY()

public:
	virtual void TreeStart(
		FStateTreeExecutionContext& Context
		) override;

	virtual void TreeStop(
		FStateTreeExecutionContext& Context
		) override;

	virtual void Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
		) override;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AGuideThreadBase> GuideThreadActorPtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGloabVariable_GuideThreadBase> GloabVariable = nullptr;
};
#pragma endregion

#pragma region 主线
UCLASS(Blueprintable)
class GUIDESYSTEM_API UGloabVariable_GuideThread_MainBase : public UObject
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
class GUIDESYSTEM_API USTE_GuideThread_MainBase : public USTE_GuideThreadBase
{
	GENERATED_BODY()

public:
	virtual void TreeStart(
		FStateTreeExecutionContext& Context
		) override;

	virtual void TreeStop(
		FStateTreeExecutionContext& Context
		) override;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AGuideThread_MainBase> GuideThreadMainActorPtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGloabVariable_GuideThread_MainBase> GloabVariable_MainPtr = nullptr;
};
#pragma endregion

#pragma region 支线
UCLASS(Blueprintable)
class GUIDESYSTEM_API UGloabVariable_GuideThread_BrandBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;
};

UCLASS(Blueprintable)
class GUIDESYSTEM_API USTE_GuideThread_BrandBase : public USTE_GuideThreadBase
{
	GENERATED_BODY()

public:
	virtual void TreeStart(
		FStateTreeExecutionContext& Context
		) override;

	virtual void TreeStop(
		FStateTreeExecutionContext& Context
		) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	AGuideThread_BranchBase* GuideThreadBranchActorPtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	UGloabVariable_GuideThread_BrandBase* GloabVariable_Brand = nullptr;
};
#pragma endregion

#pragma region 区域
UCLASS(Blueprintable)
class GUIDESYSTEM_API UGloabVariable_GuideThread_AreaBase : public UGloabVariable_GuideThreadBase
{
	GENERATED_BODY()

public:
	/**
	 * 引导生成的即时Actor
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	TArray<TObjectPtr<AActor>> TemporaryActorAry;
};

UCLASS(Blueprintable)
class GUIDESYSTEM_API USTE_GuideThread_AreaBase : public USTE_GuideThreadBase
{
	GENERATED_BODY()

public:
	virtual void TreeStart(
		FStateTreeExecutionContext& Context
		) override;

	virtual void TreeStop(
		FStateTreeExecutionContext& Context
		)  override;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AGuideThread_AreaBase> GuideThreadAreaBsaePtr = nullptr;

	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGloabVariable_GuideThread_AreaBase> GloabVariable_AreaBasePtr = nullptr;
};
#pragma endregion
