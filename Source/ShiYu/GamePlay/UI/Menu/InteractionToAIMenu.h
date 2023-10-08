// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"

#include "Common/GenerateType.h"

#include "InteractionToAIMenu.generated.h"

class ACharacterBase;

/**
 *
 */
UCLASS()
class SHIYU_API UInteractionToAIMenu : public UWidgetBase
{
	GENERATED_BODY()

public:

	void InteractionAICharacter();

//	void OnClickCollectCMD(EItemsType ItemsType);

	void OnClickUnion();

	void OnClickTheft();

	void OnClickViewBackpack();

	ACharacterBase* GetCharacter() { return TargetCharacterPtr; }

private:

	ACharacterBase* TargetCharacterPtr = nullptr;

};
