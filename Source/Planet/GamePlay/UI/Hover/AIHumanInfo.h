// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "GenerateType.h"

#include "AIHumanInfo.generated.h"

class ACharacterBase;

class UToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UAIHumanInfo : public UUserWidget
{
	GENERATED_BODY()

public:

	using FDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCampType(ECharacterCampType CharacterCampType);

	ACharacterBase* CharacterPtr = nullptr;

protected:

	void OnHPCurrentValueChanged(int32 NewVal);

	void OnHPMaxValueChanged(int32 NewVal);

private:

	void OnHPChanged();

	int32 CurrentHP = 0;

	int32 MaxHP = 0;

	bool ResetPosition(float InDeltaTime);

	FVector2D SizeBox = FVector2D::ZeroVector;

	FVector2D PreviousPt = FVector2D::ZeroVector;

	FVector2D DesiredPt = FVector2D::ZeroVector;

	float Interval = 1.f / 30.f;

	float CurrentInterval = 0.f;

	float Offset = 100.f;

	FDelegateHandle CurrentHPValueChanged;

	FDelegateHandle MaxHPValueChanged;

	FTSTicker::FDelegateHandle TickDelegateHandle;

};
