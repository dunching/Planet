// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Dynamic_WeatherBase.h"

#include "Dynamic_Weather.generated.h"

UCLASS()
class PLANET_API ADynamic_Weather : public ADynamic_WeatherBase
{
	GENERATED_BODY()

public:
	virtual bool IsNetRelevantFor(
		const AActor* RealViewer,
		const AActor* ViewTarget,
		const FVector& SrcLocation
		) const override;

};