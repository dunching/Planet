// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "EffectItem.generated.h"

struct FStreamableHandle;
class UCS_Base;

UCLASS()
class PLANET_API UEffectItem : 
	public UMyUserWidget

{
	GENERATED_BODY()

public:

	using FDataChangedHandle = TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	void SetData(UCS_Base* CharacterStatePtr);

protected:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void OnUpdate();

	void SetNum(int32 NewNum);

	void SetSetNumIsDisplay(bool bIsDisplay);

	// ��ȴ false 1~0 ����ʱ�� true 1~0
	void SetPercent(bool bIsInversion, float Percent);

	void SetPercentIsDisplay(bool bIsDisplay);

	void SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr);

	UCS_Base* CharacterStatePtr = nullptr;

	FDataChangedHandle DataChangedHandle;

};
