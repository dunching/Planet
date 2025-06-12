#include "GameState_Main.h"

#include "Dynamic_WeatherBase.h"
#include "WeatherSystem.h"
#include "Net/UnrealNetwork.h"

void AGameState_Main::BeginPlay()
{
	Super::BeginPlay();

	SetOpenWorldWeather(WeatherSettings::Clear_Skies);
}

void AGameState_Main::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
	) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, OpenWorldWeather, COND_OwnerOnly);
}

void AGameState_Main::ApplyWeather() const
{
}

void AGameState_Main::OnRep_OpenWorldWeather()
{
	// UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather_Client(OpenWorldWeather);
}

inline FGameplayTag AGameState_Main::GetOpenWorldWeather() const
{
	return OpenWorldWeather;
}

void AGameState_Main::SetOpenWorldWeather(
	const FGameplayTag& NewOpenWorldWeather
	)
{
	OpenWorldWeather = NewOpenWorldWeather;

	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather_Server(OpenWorldWeather);
}
