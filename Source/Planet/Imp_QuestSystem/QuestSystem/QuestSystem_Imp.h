// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "QuestSubSystem.h"

#include "QuestSystem_Imp.generated.h"

class AQuestChain_MainBase;

UCLASS()
class PLANET_API UQuestSubSystem_Imp : public UQuestSubSystem
{
	GENERATED_BODY()

public:
private:
	virtual TArray<TSubclassOf<AQuestChain_MainBase>> GetMainGuideThreadChaptersAry()const override;
	
	virtual TSubclassOf<AQuestChain_MainBase> GetToBeContinueGuideThread()const override;
};
