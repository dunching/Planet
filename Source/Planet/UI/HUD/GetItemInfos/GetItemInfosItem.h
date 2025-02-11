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
class PLANET_API UGetItemInfosItem : public UMyUserWidget, public IItemProxyIconInterface
{
	GENERATED_BODY()

public:

	using FOnFinished = TDelegate<void()>;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void ResetToolUIByData(const TSharedPtr<FSkillProxy>& ProxyPtr, EProxyModifyType ProxyModifyType);

	void ResetToolUIByData(const TSharedPtr < FCoinProxy>& ProxyPtr, EProxyModifyType ProxyModifyType, int32 Num);

	void ResetToolUIByData(const TSharedPtr < FConsumableProxy>& ProxyPtr, EProxyModifyType ProxyModifyType);

	void ResetToolUIByData(const TSharedPtr < FCharacterProxy>& ProxyPtr, EProxyModifyType ProxyModifyType);

	FOnFinished OnFinished;

protected:

	void SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr);

	void SetText(const FString& Text);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayThisAnimation();
	
	UFUNCTION(BlueprintCallable)
	void OnAnimationComplete();

};
