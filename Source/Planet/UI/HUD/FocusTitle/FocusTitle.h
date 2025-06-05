// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "UserWidget_Override.h"
#include "GenerateTypes.h"
#include "TemplateHelper.h"

#include "FocusTitle.generated.h"

class ACharacterBase;

class UToolIcon;
struct FOnAttributeChangeData;

/**
 * 锁定目标时，上方显示的敌人的信息
 */
UCLASS()
class PLANET_API UFocusTitle : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	using FOnValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetTargetCharacter(ACharacterBase* CharacterPtr);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCampType(ECharacterCampType CharacterCampType);

	void SwitchCantBeSelect(bool bIsCanBeSelect);

	TWeakObjectPtr<ACharacterBase> CharacterPtr = nullptr;

protected:

protected:

	void OnGameplayEffectTagCountChanged(const FGameplayTag Tag, int32 Count);

	void OnHPChanged(const FOnAttributeChangeData& CurrentValue);
	
	void SetHPChanged(float Value, float MaxValue);
	
	void OnPPChanged(const FOnAttributeChangeData& CurrentValue);

	void SetPP(float Value, float MaxValue);
	
	void OnShieldChanged(const FOnAttributeChangeData& CurrentValue);

	void SetShild(float Value, float MaxValue);
	
	void ApplyCharaterNameToTitle();

	void ApplyStatesToTitle();

	int32 CurrentHP = 0;

	int32 MaxHP = 0;

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

	TSet<FGameplayTag>TagSet;

};
