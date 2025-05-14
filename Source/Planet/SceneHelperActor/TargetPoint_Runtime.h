// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"

#include "TargetPoint_Runtime.generated.h"

class UNiagaraComponent;
class UMarkPoints;

/**
 * 指引玩家的目标点
 */
UCLASS()
class PLANET_API ATargetPoint_Runtime : public AActor
{
	GENERATED_BODY()

public:
	ATargetPoint_Runtime(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;
	
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMarkPoints>MarkPointsClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UNiagaraComponent> NiagaraComponentPtr = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UMarkPoints> MarkPointsPtr = nullptr;
};
