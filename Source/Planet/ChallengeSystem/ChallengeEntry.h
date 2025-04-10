// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/TriggerVolume.h"

#include "SceneActorInteractionInterface.h"

#include "ChallengeEntry.generated.h"

class AGuideChallengeThread;
class USceneActorInteractionComponent;

/**
 * 爬塔挑战进入点
 */
UCLASS()
class PLANET_API AChallengeEntry :
	public AActor,
	public ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:
	AChallengeEntry(const FObjectInitializer& ObjectInitializer);
	
	virtual USceneActorInteractionComponent*GetSceneActorInteractionComponent()const override;
	
	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenStartedLookAt(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenLookingAt(ACharacterBase* CharacterPtr) override;

	virtual void HasBeenEndedLookAt()override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideChallengeThread> GuideThreadChallengeActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<USceneActorInteractionComponent> SceneActorInteractionComponentPtr = nullptr;
	
};
