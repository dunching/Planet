
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/StateTreeComponent.h"

#include "GuideSystemStateTreeComponent.generated.h"

class UStateTreeComponent;

/**
 *
 */
UCLASS()
class PLANET_API UGuideSystemStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	
	static FName ComponentName;
	
	UGuideSystemStateTreeComponent(const FObjectInitializer& ObjectInitializer);

	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;

};
