// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "EffectItem.generated.h"

struct FStreamableHandle;
struct FCharacterStateInfo;
struct FActiveGameplayEffect;

class UCS_Base;
class UGameplayEffect;

UCLASS()
class PLANET_API UEffectItem : 
	public UMyUserWidget

{
	GENERATED_BODY()

public:

	using FDataChangedHandle = TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	void SetData(const FActiveGameplayEffect* InActiveGameplayEffectPtr);

private:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

	void OnUpdate();

	void SetNum(int32 NewNum);

	void SetSetNumIsDisplay(bool bIsDisplay);

	// 冷却 false 1~0 持续时间 true 1~0
	void SetPercent(bool bIsInversion, float Percent);

	void SetPercentIsDisplay(bool bIsDisplay);

	void SetTexutre();

	const FActiveGameplayEffect* ActiveGameplayEffectPtr = nullptr;

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;
	
	FActiveGameplayEffectHandle Handle;
	
	FDataChangedHandle DataChangedHandle;

};
