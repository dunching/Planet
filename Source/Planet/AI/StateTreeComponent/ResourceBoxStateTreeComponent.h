
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"

#include "Components/StateTreeComponent.h"
#include "Components/StateTreeComponentSchema.h"

#include "ResourceBoxStateTreeComponent.generated.h"

class ACharacterBase;

/**
 *
 */
UCLASS()
class PLANET_API UResourceBoxStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	
	static FName ComponentName;


	// // BEGIN IStateTreeSchemaProvider
	// TSubclassOf<UStateTreeSchema> GetSchema() const override;
	// // END
	//
	// virtual bool SetContextRequirements(FStateTreeExecutionContext& Context, bool bLogErrors = false) override;
	
};
