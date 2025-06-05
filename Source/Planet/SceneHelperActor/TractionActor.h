// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "TractionActor.generated.h"

class USplineComponent;
class UItemProxy_Description_ActiveSkill_Traction;

class ACharacterBase;

/**
 *
 */
UCLASS()
class PLANET_API ATractionPoint : public AActor
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	ATractionPoint(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
	virtual void Tick(
		float DeltaTime
	) override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	UPROPERTY(Replicated)
	TObjectPtr<UItemProxy_Description_ActiveSkill_Traction> ItemProxy_DescriptionPtr = nullptr;

protected:

	void CheckTarget();
	
	float CurrentTime = 0.f;
};
