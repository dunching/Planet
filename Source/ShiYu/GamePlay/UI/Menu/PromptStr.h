// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Common/GenerateType.h"
#include "PromptStr.generated.h"

/**
 *
 */
UCLASS()
class SHIYU_API UPromptStr : public UWidgetBase
{
	GENERATED_BODY()

public:

	void SetPromptStr(const FString& StrVal);

private:

};
