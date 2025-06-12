// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "WeatherSystem.generated.h"

class ADynamic_SkyBase;
class ADynamic_WeatherBase;

using FOnHourly = std::function<void(
	int32
	)>;

/**
 *
 */
UCLASS()
class WEATHER_API UWeatherSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UWeatherSystem* GetInstance();

	virtual void Initialize(
		FSubsystemCollectionBase& Collection
		) override;

	virtual void Deinitialize() override;

	void RegisterCallback();

	void ResetTime();

	// UFUNCTION(NetMulticast, Unreliable)
	void AdjustTime(
		const FDateTime& Time
		);

	int32 AddOnHourly(
		const FOnHourly& OnHourly
		);

	ADynamic_SkyBase* GetDynamicSky();

	ADynamic_WeatherBase* GetDynamicWeather();

protected:

private:
	void OnHoury(
		int32 Hour
		);

	FDateTime CustomTime;

	FDateTime CurrentTime;

	float AdjustTimeSpeed = 200.f;

	float OriginalSpeed = 1.f;

	ADynamic_SkyBase* Dynamic_SkyPtr = nullptr;

	ADynamic_WeatherBase* Dynamic_WeatherPtr = nullptr;
};
