// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <EnvironmentQuery/EnvQueryContext.h>

#include "EQC_Test.generated.h"

UCLASS()
class PLANET_API UEQC_Test : public UEnvQueryContext
{
	GENERATED_BODY()

public:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};