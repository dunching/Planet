// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"

#include "GetItemInfosItem.generated.h"

struct FStreamableHandle;

UCLASS()
class PLANET_API UGetItemInfosItem : public UMyUserWidget, public IUnitIconInterface
{
	GENERATED_BODY()

public:

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void ResetToolUIByData(USkillUnit* UnitPtr, bool bIsAdd);
	
	void ResetToolUIByData(UCoinUnit* UnitPtr, bool bIsAdd, int32 Num);
	
	void ResetToolUIByData(UConsumableUnit* UnitPtr, bool bIsAdd, int32 Num);
	
	void ResetToolUIByData(UCharacterUnit* UnitPtr, bool bIsAdd);

protected:

	void SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr);

	void SetText(const FString& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayThisAnimation();
	
	UFUNCTION(BlueprintCallable)
	void OnAnimationComplete();

};
