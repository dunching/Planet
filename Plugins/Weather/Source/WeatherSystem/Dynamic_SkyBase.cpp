
#include "Dynamic_SkyBase.h"

#include <Kismet/GameplayStatics.h>
#include <Components/ArrowComponent.h>

ADynamic_SkyBase::ADynamic_SkyBase(
	const FObjectInitializer& ObjectInitializer
	)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;
	SetNetUpdateFrequency(100.f);
	
	SetReplicatingMovement(true);

	bIsSpatiallyLoaded = false;
	
}

void ADynamic_SkyBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADynamic_SkyBase::AddtionalProcess()
{
}

int32 ADynamic_SkyBase::AddOnHourly(const FOnHourly& OnHourly)
{
	int32 ID = 0;
	for (int32 Index = 0; Index < std::numeric_limits<int32>::max(); Index++)
	{
		if (!HourlyMap.contains(Index))
		{
			ID = Index;
			break;
		}
	}

	HourlyMap.emplace(ID, OnHourly);

	return ID;
}

void ADynamic_SkyBase::RemoveOnHourly(int32 ID)
{
	if (HourlyMap.contains(ID))
	{
		HourlyMap.erase(ID);
	}
}

void ADynamic_SkyBase::OnHoury(int32 Hour)
{
	for (auto& Iter : HourlyMap)
	{
		if (Iter.second)
		{
			Iter.second(Hour);
		}
	}
	
	OnHourChanged(Hour);
	// 	{
	// 		const FLatentActionInfo LatentInfo(0, FMath::Rand(), TEXT("FirstCall"), this);
	// 		UKismetSystemLibrary::Delay(this, 2.f, LatentInfo);
	// 	}
	// 	{
	// 		const FLatentActionInfo LatentInfo(0, FMath::Rand(), TEXT("SecondCall"), this);
	// 		UKismetSystemLibrary::Delay(this, 3.0f, LatentInfo);
	// 	}
	// 	Refresh_Debug();
}

void ADynamic_SkyBase::FirstCall()
{
	TArray<UArrowComponent*> ArrowAry;
	GetComponents<UArrowComponent>(ArrowAry);

	for (auto Iter : ArrowAry)
	{
		Iter->SetHiddenInGame(!Iter->bHiddenInGame);
	}
}

void ADynamic_SkyBase::SecondCall()
{
	TArray<UArrowComponent*> ArrowAry;
	GetComponents<UArrowComponent>(ArrowAry);

	for (auto Iter : ArrowAry)
	{
		Iter->SetHiddenInGame(!Iter->bHiddenInGame);
	}
}

