// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "AbilitySystemComponent.h"

#include "PlanetAbilitySystemComponent.generated.h"

class UPlanetAbilitySystemComponent;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlanetAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IPlanetAbilitySystemInterface
{
	GENERATED_BODY()

public:

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const = 0;
};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPlanetAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

};