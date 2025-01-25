// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "Component/SceneActorInteractionComponent.h"
#include "SceneActorInteractionInterface.h"

#include "SceneActor.generated.h"

class UNetConnection;
class ACharacterBase;

UCLASS(BlueprintType, Blueprintable)
class USceneActorPropertyComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USceneActorPropertyComponent(const FObjectInitializer& ObjectInitializer);

	static FName ComponentName;

protected:

private:

};

UCLASS()
class PLANET_API ASceneActor :
	public AActor,
	public ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:

	ASceneActor(const FObjectInitializer& ObjectInitializer);

	virtual UNetConnection* GetNetConnection() const override;

	virtual USceneActorInteractionComponent*GetSceneActorInteractionComponent()const override;
	
	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenStartedLookAt(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenLookingAt(ACharacterBase* CharacterPtr)override;

	virtual void HasBeenEndedLookAt()override;

	template<typename Type = USceneActorPropertyComponent>
	Type* GetPropertyComponent()const { return Cast<Type>(PropertyComponentPtr); }

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneActorPropertyComponent* PropertyComponentPtr = nullptr;

private:

};
