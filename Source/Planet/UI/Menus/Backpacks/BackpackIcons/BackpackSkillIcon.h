// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BackpackIcon.h"

#include "BackpackSkillIcon.generated.h"

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

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	void SetNum(int32 NewNum);

	void SetValue(int32 Value);

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

private:

	TSharedPtr < FSkillProxy> ProxyPtr = nullptr;

};
