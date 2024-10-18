// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "GenerateType.h"

#include "GetItemInfosItem.generated.h"

struct FStreamableHandle;
struct FBasicProxy;

UCLASS()
class PLANET_API UGetItemInfosItem : public UMyUserWidget, public IUnitIconInterface
{
	GENERATED_BODY()

public:

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void ResetToolUIByData(const TSharedPtr<FSkillProxy>& UnitPtr, bool bIsAdd);

	void ResetToolUIByData(const TSharedPtr < FCoinProxy>& UnitPtr, bool bIsAdd, int32 Num);

	void ResetToolUIByData(const TSharedPtr < FConsumableProxy>& UnitPtr, EProxyModifyType ProxyModifyType);

	void ResetToolUIByData(const TSharedPtr < FCharacterProxy>& UnitPtr, bool bIsAdd);

protected:

	void SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr);

	void SetText(const FString& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayThisAnimation();
	
	UFUNCTION(BlueprintCallable)
	void OnAnimationComplete();

};
