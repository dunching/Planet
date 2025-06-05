// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "TransitionSubSystem.generated.h"

struct FPawnDataStruct;
struct FSceneTool;


/**
 * 过渡，如传送时
 */
UCLASS()
class PLANET_API UTransitionSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static UTransitionSubSystem* GetInstance();

protected:

	
	
private:

};
