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
class PLANET_API UGE_Damage : public UGameplayEffect
{
	GENERATED_BODY()
public:
	
};

UCLASS()
class PLANET_API UGE_Duration : public UGameplayEffect
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

/*
 * 造成输出后的回执
 * 比这次伤害是否使目标死亡
 */
UCLASS()
class PLANET_API UGE_Damage_Callback : public UGameplayEffect
{
	GENERATED_BODY()
public:
	
};
