// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ResourceBoxBase.h"
#include "ActorSequenceComponent.h"
#include "SceneActorInteractionInterface.h"

#include "ResourceBox.generated.h"

class UActorSequenceComponent;
class UBoxComponent;
class UWidgetComponent;

class ACharacterBase;

UCLASS()
class PLANET_API AResourceBox :
	public AResourceBoxBase
{
	GENERATED_BODY()

public:
	AResourceBox(
		const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void HasbeenInteracted(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenStartedLookAt(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenLookingAt(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenEndedLookAt() override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(
		float DeltaSeconds
		) override;

	UFUNCTION(NetMulticast, Reliable)
	void InteractionImp();

	UFUNCTION()
	void OnAnimationFinished();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UWidgetComponent* InteractionWidgetCompoentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UActorSequenceComponent* ActorSequenceComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Range = 200;
};
