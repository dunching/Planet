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
#include "STE_GuideThreadBase.h"

#include "STE_GuideThread.generated.h"

class AGuideThread;
class AGuideThread_Main;
class AGuideThread_Branch;
class AGuideThread_Area;
class AGuideThread_Challenge;
class AGuideInteractionBase;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;

#pragma region 主线
UCLASS(Blueprintable)
class PLANET_GUIDESYSTEM_API USTE_GuideThread_Main : public USTE_GuideThread_MainBase
{
	GENERATED_BODY()

};
#pragma endregion 

#pragma region 支线
UCLASS(Blueprintable)
class PLANET_GUIDESYSTEM_API USTE_GuideThread_Brand : public USTE_GuideThread_BrandBase
{
	GENERATED_BODY()

public:

};
#pragma endregion

#pragma region 区域
UCLASS(Blueprintable)
class PLANET_GUIDESYSTEM_API UGloabVariable_GuideThread_Area : public UGloabVariable_GuideThread_AreaBase
{
	GENERATED_BODY()

public:

	TArray<TWeakObjectPtr<ACharacterBase>> SpwanedCharacterAry;

};


UCLASS(Blueprintable)
class PLANET_GUIDESYSTEM_API USTE_GuideThread_Area : public USTE_GuideThread_AreaBase
{
	GENERATED_BODY()

public:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual auto TreeStop(FStateTreeExecutionContext& Context) -> void override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Content)
	TObjectPtr<AGuideThread_Area> GuideThreadAreaPtr = nullptr;

private:
	
	/**
	 * 任务之间共享数据
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UGloabVariable_GuideThread_Area>GloabVariable_AreaPtr  = nullptr;

};
#pragma endregion
