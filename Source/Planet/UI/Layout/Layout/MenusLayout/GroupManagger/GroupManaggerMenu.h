// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MenuInterface.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"

#include "GroupManaggerMenu.generated.h"

struct FSceneObjContainer;

/**
 *
 */
UCLASS()
class PLANET_API UGroupManaggerMenu :
	public UUserWidget_Override, 
	public IMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	AHumanCharacter* HumanCharacterPtr = nullptr;

protected:

	virtual void EnableMenu()override;

	virtual void DisEnableMenu()override;

	virtual EMenuType GetMenuType()const override final;
	
private:

	void ResetGroupmates();

};
