// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GameFramework/GameState.h"

#include "GameState_Main.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API AGameState_Main : public AGameStateBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	FGameplayTag GetOpenWorldWeather()const;
	
	void SetOpenWorldWeather(const FGameplayTag&NewOpenWorldWeather);
	
private:
	
	void ApplyWeather()const;
	
	UFUNCTION()
	void OnRep_OpenWorldWeather();

	/*
	 *	角色进入副本之前，开放世界的天气
	*/
	UPROPERTY(ReplicatedUsing = OnRep_OpenWorldWeather)
	FGameplayTag OpenWorldWeather;
	
};

