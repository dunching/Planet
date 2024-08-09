// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "Dynamic_Weather.generated.h"

UCLASS(Blueprintable)
class UDS_Weather_Settings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

};

UCLASS()
class WEATHER_API ADynamic_Weather : public AActor
{
	GENERATED_BODY()

public:

	ADynamic_Weather(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	void UpdateWeather(const FGameplayTag& WeatherType);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSeasonCPP();

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeatherCPP(UPrimaryDataAsset* DS_Weather_SettingsPtr);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<UPrimaryDataAsset>>WeatherMap;
};

namespace WeatherSettings
{
	const FGameplayTag Clear_Skies = FGameplayTag::RequestGameplayTag(TEXT("Weather.Clear_Skies"));

	const FGameplayTag Cloudy = FGameplayTag::RequestGameplayTag(TEXT("Weather.Cloudy"));

	const FGameplayTag Foggy = FGameplayTag::RequestGameplayTag(TEXT("Weather.Foggy"));

	const FGameplayTag Overcast = FGameplayTag::RequestGameplayTag(TEXT("Weather.Overcast"));

	const FGameplayTag Partly_Cloudy = FGameplayTag::RequestGameplayTag(TEXT("Weather.Partly_Cloudy"));

	const FGameplayTag Rain = FGameplayTag::RequestGameplayTag(TEXT("Weather.Rain"));

	const FGameplayTag Rain_Light = FGameplayTag::RequestGameplayTag(TEXT("Weather.Rain_Light"));

	const FGameplayTag Rain_Thunderstorm = FGameplayTag::RequestGameplayTag(TEXT("Weather.Rain_Thunderstorm"));

	const FGameplayTag Sand_Dust_Calm = FGameplayTag::RequestGameplayTag(TEXT("Weather.Sand_Dust_Calm"));

	const FGameplayTag Sand_Dust_Storm = FGameplayTag::RequestGameplayTag(TEXT("Weather.Sand_Dust_Storm"));

	const FGameplayTag Snow = FGameplayTag::RequestGameplayTag(TEXT("Weather.Snow"));

	const FGameplayTag Snow_Blizzard = FGameplayTag::RequestGameplayTag(TEXT("Weather.Snow_Blizzard"));

	const FGameplayTag Snow_Light = FGameplayTag::RequestGameplayTag(TEXT("Weather.Snow_Light"));
}