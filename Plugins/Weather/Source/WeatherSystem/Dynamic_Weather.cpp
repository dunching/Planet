
#include "Dynamic_Weather.h"

#include <Kismet/GameplayStatics.h>

ADynamic_Weather::ADynamic_Weather(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	WeatherMap.Add(WeatherSettings::Clear_Skies, nullptr);
	WeatherMap.Add(WeatherSettings::Cloudy, nullptr);
	WeatherMap.Add(WeatherSettings::Foggy, nullptr);
	WeatherMap.Add(WeatherSettings::Overcast, nullptr);
	WeatherMap.Add(WeatherSettings::Partly_Cloudy, nullptr);
	WeatherMap.Add(WeatherSettings::Rain, nullptr);
	WeatherMap.Add(WeatherSettings::Rain_Light, nullptr);
	WeatherMap.Add(WeatherSettings::Rain_Thunderstorm, nullptr);
	WeatherMap.Add(WeatherSettings::Sand_Dust_Calm, nullptr);
	WeatherMap.Add(WeatherSettings::Sand_Dust_Storm, nullptr);
	WeatherMap.Add(WeatherSettings::Snow, nullptr);
	WeatherMap.Add(WeatherSettings::Snow_Blizzard, nullptr);
	WeatherMap.Add(WeatherSettings::Snow_Light, nullptr);
}

void ADynamic_Weather::BeginPlay()
{
	Super::BeginPlay();
}

void ADynamic_Weather::UpdateWeather(const FGameplayTag& WeatherType)
{
	const auto Iter = WeatherMap.Find(WeatherType);
	if (Iter)
	{
		UpdateWeatherCPP(Iter->LoadSynchronous());
	}
}
