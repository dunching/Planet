// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "Consumable_Base.h"
#include "Planet_Consumable_Base.h"
#include "SceneActorInteractionInterface.h"

#include "Consumable_Generic.generated.h"

class UStaticMeshComponent;

UCLASS()
class PLANET_API AConsumable_Generic : public APlanet_Consumable_Base
{
	GENERATED_BODY()

public:

	AConsumable_Generic(const FObjectInitializer& ObjectInitializer);

	virtual void HasbeenInteracted(
		ACharacterBase* CharacterPtr
		) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName Socket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capture Point")
	UStaticMeshComponent* StaticComponentPtr = nullptr;

};
