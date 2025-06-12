

#include "Dynamic_Weather.h"

#include "PlanetPlayerController.h"
#include "PlanetPlayerState.h"

bool ADynamic_Weather::IsNetRelevantFor(
	const AActor* RealViewer,
	const AActor* ViewTarget,
	const FVector& SrcLocation
	) const
{
 	auto PCPtr = Cast<APlanetPlayerController>(RealViewer);
	if (PCPtr)
	{
		auto PSPtr = PCPtr->GetPlayerState<APlanetPlayerState>();
		if (PSPtr && PSPtr->GetIsInChallenge())
		{
			return false;
		}
	}
 	
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}
