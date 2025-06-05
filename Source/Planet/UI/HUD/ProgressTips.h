// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "ProgressTips.generated.h"

UCLASS()
class PLANET_API UProgressTips : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetWaitTime(int32 val);

protected:

	int32 WaitTime = -1;

	float CurrentTime = 0.f;

	void SetPercent(float Percent);

};
