
#include "Hall_LevelScript.h"

#include <type_traits>

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
}

void AMain_LevelScriptActor::ClearWorldProcessCache()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(),OutActors);
	for (auto Iter : OutActors)
	{
		if (Iter)
		{
			Iter->Destroy();
		}
	}
}
