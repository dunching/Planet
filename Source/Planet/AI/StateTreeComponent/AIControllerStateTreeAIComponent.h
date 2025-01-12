
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"

#include "Components/StateTreeComponent.h"

#include "AIControllerStateTreeAIComponent.generated.h"

class ACharacterBase;

/**
 *
 */
UCLASS()
class PLANET_API UAIControllerStateTreeAIComponent : public UStateTreeAIComponent
{
	GENERATED_BODY()

public:
	
	static FName ComponentName;
};
