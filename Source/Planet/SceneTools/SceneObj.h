// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "Component/ItemInteractionComponent.h"
#include "SceneObjInteractionInterface.h"

#include "SceneObj.generated.h"

class UNetConnection;
class ACharacterBase;

UCLASS(BlueprintType, Blueprintable)
class USceneObjPropertyComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	USceneObjPropertyComponent(const FObjectInitializer& ObjectInitializer);

	static FName ComponentName;

protected:

private:

};

UCLASS()
class PLANET_API ASceneObj :
	public AActor,
	public ISceneObjInteractionInterface
{
	GENERATED_BODY()

public:

	ASceneObj(const FObjectInitializer& ObjectInitializer);

	virtual UNetConnection* GetNetConnection() const override;

	virtual void Interaction(ACharacterBase* CharacterPtr)override;

	virtual void StartLookAt(ACharacterBase* CharacterPtr)override;

	virtual void LookingAt(ACharacterBase* CharacterPtr)override;

	virtual void EndLookAt()override;

	template<typename Type = USceneObjPropertyComponent>
	Type* GetPropertyComponent()const { return Cast<Type>(PropertyComponentPtr); }

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneObjPropertyComponent* PropertyComponentPtr = nullptr;

private:

};
