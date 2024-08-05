// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>

#include "SceneObjSubSystem.generated.h"

UCLASS()
class PLANET_API USceneObjSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static USceneObjSubSystem* GetInstance();

	APostProcessVolume* GetSkillPost()const;

};