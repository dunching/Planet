// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "EffectItem.generated.h"

struct FStreamableHandle;
struct FCharacterStateInfo;
struct FActiveGameplayEffect;

class UCS_Base;
class UGameplayEffect;
class UAbilitySystemComponent;

UCLASS()
class PLANET_API UEffectItem : 
	public UUserWidget_Override

{
	GENERATED_BODY()

public:

	using FDataChangedHandle = TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	void SetData(
		const TObjectPtr<UAbilitySystemComponent>& AbilitySystemComponentPtr,
		FActiveGameplayEffectHandle NewActiveGameplayEffectHandle
		);

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

	TSharedPtr<FCharacterStateInfo> CharacterStateInfoSPtr = nullptr;
	
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponentPtr = nullptr;
	
	FActiveGameplayEffectHandle Handle;
	
	FDataChangedHandle DataChangedHandle;

};
