// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "DestroyProgress.generated.h"

class ABuildingBase;

/**
 *
 */
UCLASS()
class SHIYU_API UDestroyProgress : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetLookAtObject(ABuildingBase*BuildingPtr);

	void SetProgress(uint8 Progress);

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

	ABuildingBase* BuildingItemPtr = nullptr;

};
