// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeExecutionContext.h"

#include "GenerateType.h"

#include "STE_HumanTeammate.generated.h"

class AHumanCharacter;
class AHumanAIController;

UCLASS(Blueprintable)
class PLANET_API USTE_HumanTeammate : public UStateTreeEvaluatorBlueprintBase
{
	GENERATED_BODY()
public:

protected:

	virtual void TreeStart(FStateTreeExecutionContext& Context)override;

	virtual void TreeStop(FStateTreeExecutionContext& Context)override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime)override;

	void OnTeamOptionChanged(ETeammateOption TeammateOption);

	void OnTeamChanged();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	ETeammateOption TeammateOption = ETeammateOption::kFollow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	AHumanCharacter* HumanCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	AHumanCharacter* TargetCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Context)
	AHumanAIController* HumanAIControllerPtr = nullptr;

	TCallbackHandleContainer<void(ETeammateOption, AHumanCharacter*)>::FCallbackHandleSPtr TeammateOptionChangedDelegate;
	
	TCallbackHandleContainer<void()>::FCallbackHandleSPtr TeammateChangedDelegate;

};
