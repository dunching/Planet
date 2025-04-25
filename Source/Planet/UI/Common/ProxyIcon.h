// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"

#include "ProxyIcon.generated.h"

class UTaskItem;
class UTaskItemCategory;

/**
 *
 */
UCLASS()
class PLANET_API UProxyIcon :
	public UMyUserWidget
{
	GENERATED_BODY()

public:
	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
	);

private:
	virtual void SetItemType();

	TSharedPtr<FBasicProxy> BasicProxyPtr = nullptr;
};
