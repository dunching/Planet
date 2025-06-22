// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BackpackIcon.h"

#include "BackpackSkillIcon.generated.h"

class UTextBlock;

struct FStreamableHandle;

struct FBasicProxy;
struct FSkillProxy;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackSkillIcon :
	public UBackpackIcon
{
	GENERATED_BODY()

public:

	UBackpackSkillIcon(const FObjectInitializer& ObjectInitializer);

	virtual void NativeDestruct()override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	void SetNum(int32 NewNum);

	void SetValue(int32 Value);

	/**
	 * < 0 不显示
	 * @param InLevel 
	 */
	void SetLevel(int32 InLevel);

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText = nullptr;

	TSharedPtr < FSkillProxy> ProxyPtr = nullptr;

	TOnValueChangedCallbackContainer<uint8>::FCallbackHandleSPtr DelageteHandle;
};
