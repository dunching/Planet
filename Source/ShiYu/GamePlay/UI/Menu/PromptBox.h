// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Common/GenerateType.h"
#include "PromptBox.generated.h"

/**
 *
 */
UCLASS()
class SHIYU_API UPromptBox : public UWidgetBase
{
	GENERATED_BODY()

public:

	void SetPromptStr(const TArray<FString>& TextAry);

private:

};
