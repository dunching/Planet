// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "SceneActor.generated.h"

class UNetConnection;
class ACharacterBase;

UCLASS(BlueprintType, Blueprintable)
class SCENEACTOR_API USceneActorPropertyComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USceneActorPropertyComponent(const FObjectInitializer& ObjectInitializer);

	static FName ComponentName;

protected:

private:

};

UCLASS()
class SCENEACTOR_API ASceneActor :
	public AActor
{
	GENERATED_BODY()

public:

	ASceneActor(const FObjectInitializer& ObjectInitializer);

	virtual UNetConnection* GetNetConnection() const override;

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
