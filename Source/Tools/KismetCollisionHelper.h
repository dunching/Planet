
#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include <Kismet/BlueprintFunctionLibrary.h>

#include "KismetCollisionHelper.generated.h"

UCLASS()
class UKismetCollisionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static TOOLS_API TArray<FHitResult> OverlapMultiSectorByObjectType(
		const UWorld* World,
		const FVector& Start,
		const FVector& End,
		int32 Angle,
		int32 LineNum,
		const FCollisionObjectQueryParams& ObjectQueryParams,
		const FCollisionQueryParams& Params
	);

};