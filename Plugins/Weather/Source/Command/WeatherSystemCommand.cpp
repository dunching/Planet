
#include "WeatherSystemCommand.h"

#include <Kismet/KismetStringLibrary.h>
#include <GameplayTagContainer.h>

#include "WeatherSystem.h"
#include "Dynamic_WeatherBase.h"

void WeatherSystemCommand::AdjustHour(const TArray< FString >& Args)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	const auto Time = FDateTime::Now();

	UWeatherSystem::GetInstance()->AdjustTime(FDateTime(1, 1, 1, UKismetStringLibrary::Conv_StringToInt(Args[0])));
}

void WeatherSystemCommand::AdjustWeather(const TArray< FString >& Args)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	const auto Time = FDateTime::Now();

	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather_Server(FGameplayTag::RequestGameplayTag(*Args[0]));
}

