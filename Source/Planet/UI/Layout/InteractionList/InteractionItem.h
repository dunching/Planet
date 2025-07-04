// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "InteractionItem.generated.h"

class AGuideActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class AQuestInteractionBase;

using FOnInteractionItemClicked = TMulticastDelegate<void()>;

UCLASS()
class PLANET_API UInteractionItem :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	void SetData(const FOnInteractionItemClicked &InOnInteractionItemClicked);

protected:

	virtual void Enable() override;
	
	virtual void DisEnable() override;

	UFUNCTION()
	void OnClicked();
	
	FOnInteractionItemClicked OnInteractionItemClicked;
	
};
