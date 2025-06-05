// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"
#include <AIController.h>

#include "GenerateTypes.h"
#include "HoverWidgetBase.h"

#include "TaskPromt.generated.h"

class ACharacterBase;

class UToolIcon;

/**
 * 当NPC有可解接取的支线时显示的头顶上的提示
 */
UCLASS()
class PLANET_API UTaskPromt : public UHoverWidgetBase
{
	GENERATED_BODY()

public:
	virtual FVector GetHoverPosition() override;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

protected:
	UPROPERTY(EditAnywhere)
	FVector Offsert = FVector(0, 0, 50);

private:
};
