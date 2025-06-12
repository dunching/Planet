#include "Hall_LevelScript.h"

#include <type_traits>

#include "Dynamic_WeatherBase.h"
#include "WeatherSystem.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

void AHall_LevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMain_LevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	ClearWorldProcessCache();
	
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_DedicatedServer)
	{
	}
#endif
	
#if UE_EDITOR || UE_CLIENT
	if (GetNetMode() == NM_Client)
	{
	}
#endif
}

void AMain_LevelScriptActor::ClearWorldProcessCache()
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), OutActors);
		for (auto Iter : OutActors)
		{
			if (Iter)
			{
				Iter->Destroy();
			}
		}
	}
#endif
}
