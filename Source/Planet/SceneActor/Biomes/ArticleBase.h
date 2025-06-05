// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GenerateTypes.h"
#include "SceneActor.h"
#include "SceneActorInteractionInterface.h"

#include "ArticleBase.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class PLANET_API URawMaterialInteractionComponent : public USceneActorPropertyComponent
{
	GENERATED_BODY()

public:
	URawMaterialInteractionComponent(
		const FObjectInitializer& ObjectInitializer
		);

	EPickType GetPickType() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
	EPickType PickType = EPickType::kNone;
};

UCLASS()
class PLANET_API ARawMaterialBase : public ASceneActor,
                                    public ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:
	ARawMaterialBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void BeginPlay() override;

	virtual USceneActorInteractionComponent* GetSceneActorInteractionComponent() const override;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SceneComp")
	USceneComponent* SceneCompPtr = nullptr;

	static FName SceneCompName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMeshComp")
	UStaticMeshComponent* StaticMeshCompPtr = nullptr;

	static FName StaticMeshCompName;

private:
};
