// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"

#include "OptionList.generated.h"

class AGuideActor;
class UOptionItem;
class ACharacterBase;
class AHumanCharacter_AI;

UCLASS()
class PLANET_API UOptionList :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void ResetUIByData() override;

	void UpdateDisplay(AHumanCharacter_AI* InTargetCharacterPtr);
	
	void CloseUI();

protected:
	
	bool ResetPosition(float InDeltaTime);

	AHumanCharacter_AI* TargetCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Class")
	TSubclassOf<UOptionItem>InteractionItemClass;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};
