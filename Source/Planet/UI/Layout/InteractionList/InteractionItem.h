// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "InteractionItem.generated.h"

class AGuideActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class AGuideInteractionActor;

UCLASS()
class PLANET_API UInteractionItem :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void ResetUIByData() override;

	void SetData(const TSubclassOf<AGuideInteractionActor>&TaskNode);

protected:

	UFUNCTION()
	void OnClicked();
	
	TSubclassOf<AGuideInteractionActor> TaskNode;

};
