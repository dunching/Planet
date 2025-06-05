
#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include <Kismet/BlueprintFunctionLibrary.h>

#include "GameplayCameraCommon.h"

#include "GameplayCameraHelper.generated.h"

class AHumanCharacter_Player;

UCLASS()
class UGameplayCameraHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static PLANET_API void SwitchGaplayCameraType(
		const TObjectPtr<AHumanCharacter_Player>& PlayerCharacterPtr,
		ECameraType CameraType
	);

};