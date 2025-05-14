// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "InteractionList.generated.h"

class AGuideActor;
class UInteractionItem;
class ACharacterBase;
class AHumanCharacter_AI;

UCLASS()
class PLANET_API UInteractionList :
	public UMyUserWidget,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void UpdateDisplay(AHumanCharacter_AI* InTargetCharacterPtr);
	
	void CloseUI();

protected:
	
	bool ResetPosition(float InDeltaTime);

	AHumanCharacter_AI* TargetCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Class")
	TSubclassOf<UInteractionItem>InteractionItemClass;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};
