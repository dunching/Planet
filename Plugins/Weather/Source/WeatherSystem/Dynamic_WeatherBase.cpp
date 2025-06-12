#include "Dynamic_WeatherBase.h"

#include <Kismet/GameplayStatics.h>

ADynamic_WeatherBase::ADynamic_WeatherBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;
	SetNetUpdateFrequency(100.f);

	SetReplicatingMovement(true);

	bIsSpatiallyLoaded = false;

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

void ADynamic_WeatherBase::BeginPlay()
{
	Super::BeginPlay();

	// ForceNetUpdate();
	// SetNetDormancy(ENetDormancy::DORM_DormantAll);
}

void ADynamic_WeatherBase::Tick(
	float DeltaSeconds
	)
{
	Super::Tick(DeltaSeconds);
}

void ADynamic_WeatherBase::EndPlay(
	const EEndPlayReason::Type EndPlayReason
	)
{
	Super::EndPlay(EndPlayReason);
}

inline void ADynamic_WeatherBase::ForceNetUpdate()
{
	Super::ForceNetUpdate();
}

void ADynamic_WeatherBase::UpdateWeather_Server_Implementation(
	const FGameplayTag& WeatherType
	)
{
	const auto Iter = WeatherMap.Find(WeatherType);
	if (Iter)
	{
		CurrentWeather = WeatherType;
		UpdateWeatherCPP_Server(Iter->LoadSynchronous(), 3.f);
	}
}

void ADynamic_WeatherBase::UpdateWeather_Client_Implementation(
	const FGameplayTag& WeatherType
	)
{
	const auto Iter = WeatherMap.Find(WeatherType);
	if (Iter)
	{
		CurrentWeather = WeatherType;
		UpdateWeatherCPP_Client(Iter->LoadSynchronous(), 3.f);
	}
}

bool ADynamic_WeatherBase::IsNetRelevantFor(
	const AActor* RealViewer,
	const AActor* ViewTarget,
	const FVector& SrcLocation
	) const
{
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

FGameplayTag ADynamic_WeatherBase::GetCurrentWeather() const
{
	return CurrentWeather;
}
