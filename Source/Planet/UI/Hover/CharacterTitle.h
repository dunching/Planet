// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "MyUserWidget.h"
#include "GenerateType.h"

#include "CharacterTitle.generated.h"

class ACharacterBase;

class UToolIcon;
struct FOnAttributeChangeData;

/**
 *
 */
UCLASS()
class PLANET_API UCharacterTitle : public UMyUserWidget
{
	GENERATED_BODY()

public:
	using FOnValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCampType(ECharacterCampType CharacterCampType);

	void SwitchCantBeSelect(bool bIsCanBeSelect);

	void SetData(ACharacterBase* CharacterPtr);
	
protected:

	ACharacterBase* CharacterPtr = nullptr;

protected:
	void OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count);

	void OnHPChanged(const FOnAttributeChangeData&);

	void SetHPChanged(float Value, float MaxValue);

	void OnPPChanged(const FOnAttributeChangeData&);

	void SetPPChanged(float Value, float MaxValue);

	void OnShieldChanged(const FOnAttributeChangeData&);

	void SetShieldChanged(float Value, float MaxValue);

	void ApplyCharaterNameToTitle();

	void ApplyStatesToTitle();

	bool ResetPosition(float InDeltaTime);

	FVector2D PreviousPt = FVector2D::ZeroVector;

	FVector2D DesiredPt = FVector2D::ZeroVector;

	FVector2D Size = FVector2D::ZeroVector;

	float Interval = 1.f / 30.f;

	float CurrentInterval = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Offset = 0.f;

	float HalfHeight = 0.f;

	float Scale = 1.f;

	FOnValueChangedDelegateHandle CurrentHPValueChanged;

	FOnValueChangedDelegateHandle MaxHPValueChanged;

	TArray<FOnValueChangedDelegateHandle> ValueChangedAry;

	FTSTicker::FDelegateHandle TickDelegateHandle;

	FDelegateHandle OnGameplayEffectTagCountChangedHandle;

	TSet<FGameplayTag> TagSet;
};
