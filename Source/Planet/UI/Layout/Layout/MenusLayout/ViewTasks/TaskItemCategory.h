// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "QuestChainType.h"
#include "MenuInterface.h"

#include "TaskItemCategory.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UTaskItemCategory :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:

	void SetTaskType(EQuestChainType GuideThreadType);
	
};
