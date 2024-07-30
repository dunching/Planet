// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GravityAIController.h"

#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UPlanetAbilitySystemComponent;
class UGroupMnaggerComponent;
class UGourpmateUnit;
class ACharacterBase;

UCLASS()
class PLANET_API APlanetAIController : 
	public AGravityAIController, 
	public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;
	
	UPROPERTY(Transient)
	UGourpmateUnit* GourpMateUnitPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RowName")
	FGameplayTag RowName = FGameplayTag::EmptyTag;

protected:

private:

};
