// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "AbilitySystemComponent.h"

#include "GE_Common.generated.h"

UCLASS()
class PLANET_API UGE_Reply : public UGameplayEffect
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class PLANET_API UGE_Running : public UGameplayEffect
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FScalableFloat MoveSpeedOffset;

};

UCLASS()
class PLANET_API UGE_CancelRunning : public UGameplayEffect
{
	GENERATED_BODY()
public:
	
};