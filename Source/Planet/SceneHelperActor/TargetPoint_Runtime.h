// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TargetPoint_Runtime.generated.h"

class UNiagaraComponent;
class UWidgetComponent;

/**
 *
 */
UCLASS()
class PLANET_API ATargetPoint_Runtime : public AActor
{
	GENERATED_BODY()

public:

	ATargetPoint_Runtime(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UNiagaraComponent> NiagaraComponentPtr = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UWidgetComponent> WidgetComponentPtr = nullptr;

};
