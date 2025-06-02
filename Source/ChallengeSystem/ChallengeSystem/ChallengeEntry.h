// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SceneActor.h"
#include "SceneActorInteractionComponent.h"
#include "SceneActorInteractionInterface.h"

#include "ChallengeEntry.generated.h"

class AGuideInteractionBase;
class AGuideThread_Challenge;
class USceneActorInteractionComponent;
class UWidgetComponent;
class UBoxComponent;

/**
 * 爬塔挑战进入点
 */
UCLASS()
class CHALLENGESYSTEM_API AChallengeEntry :
	public ASceneActor,
	public ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:
	AChallengeEntry(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	virtual USceneActorInteractionComponent*GetSceneActorInteractionComponent()const override;
	
	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenStartedLookAt(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenLookingAt(ACharacterBase* CharacterPtr) override;

	virtual void HasBeenEndedLookAt()override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideThread_Challenge> GuideThreadChallengeActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<USceneActorInteractionComponent> SceneActorInteractionComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UWidgetComponent* InteractionWidgetCompoentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComponentPtr = nullptr;

};

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class CHALLENGESYSTEM_API USceneChallengeEntryInteractionComponent : public USceneActorInteractionComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AChallengeEntry;

	virtual void StartInteractionItem(const TSubclassOf<AGuideInteractionBase>& Item)override;
	
};
