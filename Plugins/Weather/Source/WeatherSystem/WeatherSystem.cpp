#include "WeatherSystem.h"

#include "Kismet/GameplayStatics.h"
#include <Subsystems/SubsystemBlueprintLibrary.h>
#include "Components/TimelineComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Materials/MaterialParameterCollection.h"

#include "Dynamic_WeatherBase.h"
#include "Dynamic_SkyBase.h"

UWorld* GetWorldImp()
{
#if WITH_EDITOR
	if (GEditor->IsPlayingSessionInEditor())
	{
		auto Ptr = GWorld;
		return GEngine->GetCurrentPlayWorld() ? GEngine->GetCurrentPlayWorld() : GEditor->PlayWorld.Get();
	}
	else
	{
		return GEditor->PlayWorld;
	}
#else
	return GEngine->GetCurrentPlayWorld();
#endif
}

UWeatherSystem* UWeatherSystem::GetInstance()
{
	return Cast<UWeatherSystem>(
	                            USubsystemBlueprintLibrary::GetWorldSubsystem(
	                                                                          GetWorldImp(),
	                                                                          UWeatherSystem::StaticClass()
	                                                                         )
	                           );
}

void UWeatherSystem::Initialize(
	FSubsystemCollectionBase& Collection
	)
{
	Super::Initialize(Collection);
}

void UWeatherSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UWeatherSystem::ResetTime()
{
	// 	if (GetDynamicSky())
	// 	{
	// 		GetDynamicSky()->SetUseRealWorldTime(true);
	// 	}

	const auto Now = FDateTime::Now();

	AdjustTime(FDateTime(Now.GetYear(), Now.GetMonth(), 10));
}

void UWeatherSystem::AdjustTime(
	const FDateTime& Time
	)
{
	CustomTime = Time;

	if (CustomTime.GetHour() != CurrentTime.GetHour())
	{
		GetDynamicSky()->SetTimeSpeed(AdjustTimeSpeed);
	}
}

void UWeatherSystem::RegisterCallback()
{
	if (GetDynamicSky())
	{
		AddOnHourly(std::bind(&ThisClass::OnHoury, this, std::placeholders::_1));
	}
}

void UWeatherSystem::OnHoury(
	int32 Hour
	)
{
	auto Now = FDateTime::Now();

	CurrentTime = FDateTime(Now.GetYear(), Now.GetMonth(), Now.GetDay(), Hour);

	if (Hour == CustomTime.GetHour())
	{
		GetDynamicSky()->SetTimeSpeed(OriginalSpeed);
	}

	// 	auto Global_MPC = UAssetsRef::GetInstance()->Global_MPC.LoadSynchronous();
	// 
	// 	UMaterialParameterCollectionInstance* LandscapeCollectionInstance = GetWorld()->GetParameterCollectionInstance(Global_MPC);
	// 	if (LandscapeCollectionInstance)
	// 	{
	// //		LandscapeCollectionInstance->SetScalarParameterValue(*Global_MPC::Hour, Hour);
	// 	}
}

int32 UWeatherSystem::AddOnHourly(
	const FOnHourly& OnHourly
	)
{
	if (GetDynamicSky())
	{
		return GetDynamicSky()->AddOnHourly(OnHourly);
	}
	return -1;
}

ADynamic_SkyBase* UWeatherSystem::GetDynamicSky()
{
	if (!Dynamic_SkyPtr)
	{
		auto LevelAry = GetWorld()->GetLevels();
		for (auto Iter : LevelAry)
		{
			bool bIsBreak = false;
			for (auto SecondIter : Iter->Actors)
			{
				Dynamic_SkyPtr = Cast<ADynamic_SkyBase>(SecondIter);
				if (Dynamic_SkyPtr)
				{
					bIsBreak = true;
					break;
				}
			}
			if (bIsBreak)
			{
				break;
			}
		}
	}
	return Dynamic_SkyPtr;
}

ADynamic_WeatherBase* UWeatherSystem::GetDynamicWeather()
{
	if (!Dynamic_WeatherPtr)
	{
		auto LevelAry = GetWorld()->GetLevels();
		for (auto Iter : LevelAry)
		{
			bool bIsBreak = false;
			for (auto SecondIter : Iter->Actors)
			{
				Dynamic_WeatherPtr = Cast<ADynamic_WeatherBase>(SecondIter);
				if (Dynamic_WeatherPtr)
				{
					bIsBreak = true;
					break;
				}
			}
			if (bIsBreak)
			{
				break;
			}
		}
	}
	return Dynamic_WeatherPtr;
}
