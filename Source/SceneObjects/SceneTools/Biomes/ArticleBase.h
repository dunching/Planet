// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SceneObj.h"

#include "ArticleBase.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class SCENEOBJECTS_API URawMaterialInteractionComponent : public USceneObjPropertyComponent
{
	GENERATED_BODY()

public:

	URawMaterialInteractionComponent(const FObjectInitializer& ObjectInitializer);

	EPickType GetPickType()const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		EPickType PickType = EPickType::kNone;

};

UCLASS()
class SCENEOBJECTS_API ARawMaterialBase : public ASceneObj
{
	GENERATED_BODY()

public:

	ARawMaterialBase(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SceneComp")
		USceneComponent* SceneCompPtr = nullptr;

	static FName SceneCompName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticMeshComp")
		UStaticMeshComponent* StaticMeshCompPtr = nullptr;

	static FName StaticMeshCompName;

private:

};
