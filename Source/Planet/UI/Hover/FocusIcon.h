// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"

#include "FocusIcon.generated.h"

class ACharacterBase;

class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UFocusIcon : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	ACharacterBase* TargetCharacterPtr = nullptr;

protected:

private:

	bool ResetPosition(float InDeltaTime);

	FTSTicker::FDelegateHandle TickDelegateHandle;

	FVector2D SizeBox = FVector2D::ZeroVector;

};
