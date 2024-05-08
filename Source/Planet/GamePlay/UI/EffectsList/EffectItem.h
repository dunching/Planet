// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "CharacterAttributesComponent.h"
#include "UIInterfaces.h"

#include "EffectItem.generated.h"

struct FStreamableHandle;

UCLASS()
class PLANET_API UEffectItem : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetNum(int32 NewNum);

	void SetSetNumIsDisplay(bool bIsDisplay);

	// ��ȴ false 1~0 ����ʱ�� true 1~0
	void SetPercent(bool bIsInversion, float Percent);

	void SetPercentIsDisplay(bool bIsDisplay);

	void SetTexutre(const TSoftObjectPtr<UTexture2D> & TexturePtr);

protected:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
