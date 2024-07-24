// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"

#include "PlanetPlayerState.generated.h"

class UPlanetAbilitySystemComponent;
class UHoldingItemsComponent;

UCLASS()
class PLANET_API APlanetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	APlanetPlayerState();

	UHoldingItemsComponent* GetHoldingItemsComponent();

protected:

	virtual void BeginPlay() override;

	void InitialData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;
	
private:

};
