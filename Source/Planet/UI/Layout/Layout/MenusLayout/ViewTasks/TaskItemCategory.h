// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "GuideThreadType.h"
#include "MenuInterface.h"

#include "TaskItemCategory.generated.h"

class UTalentIcon;

/**
 *
 */
UCLASS()
class PLANET_API UTaskItemCategory :
	public UMyUserWidget
{
	GENERATED_BODY()

public:

	void SetTaskType(EGuideThreadType GuideThreadType);
	
};
