// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>

#include "SceneActorSubSystem.generated.h"

UCLASS()
class PLANET_API USceneActorSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static USceneActorSubSystem* GetInstance();

	APostProcessVolume* GetSkillPost()const;

};