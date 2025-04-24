// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "MyUserWidget.h"
#include "GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "CoinList.generated.h"

class UCoinInfo;
struct FCoinProxy;

/**
 *
 */
UCLASS()
class PLANET_API UCoinList : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void ResetUIByData(const TMap<FGameplayTag, TSharedPtr<FCoinProxy>>& CoinMap);

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCoinInfo>CoinInfoClass;

};
