// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "GuideSubSystem.h"

#include "GuideSubSystem_Imp.generated.h"

class AGuideThread_MainBase;

UCLASS()
class PLANET_API UGuideSubSystem_Imp : public UGuideSubSystem
{
	GENERATED_BODY()

public:
private:
	virtual TArray<TSubclassOf<AGuideThread_MainBase>> GetMainGuideThreadChaptersAry()const override;
	
	virtual TSubclassOf<AGuideThread_MainBase> GetToBeContinueGuideThread()const override;
};
