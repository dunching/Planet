// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Teleport.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWidgetComponent;
class UBillboardComponent;
class UGameplayEffect;
class USphereComponent;

/**
 *
 */
UCLASS()
class PLANET_API ATeleport : public AActor
{
	GENERATED_BODY()

public:
	ATeleport(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void BeginPlay() override;

	FTransform GetLandTransform() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<UStaticMeshComponent> StaticComponentPtr = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	TObjectPtr<USceneComponent> LandPtComponentPtr = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USphereComponent> GEAreaComponentPtr = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> WidgetComponentPtr = nullptr;
#endif

	// 靠近传送点时治疗角色的GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGameplayEffect> ReplyGEClass;

	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
