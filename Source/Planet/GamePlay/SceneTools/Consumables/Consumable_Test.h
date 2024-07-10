// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "Consumable_Base.h"

#include "Consumable_Test.generated.h"

class UStaticMeshComponent;

UCLASS()
class PLANET_API AConsumable_Test : public AConsumable_Base
{
	GENERATED_BODY()

public:

	AConsumable_Test(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture Point")
	UStaticMeshComponent* StaticComponentPtr = nullptr;

};
