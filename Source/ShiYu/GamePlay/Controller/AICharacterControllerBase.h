// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"

#include "AICharacterControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class SHIYU_API AAICharacterControllerBase : public AAIController
{
	GENERATED_BODY()

public:

protected:

	virtual void BeginPlay() override;

};
