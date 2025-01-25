// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ArticleBase.h"
#include "Component/SceneActorInteractionComponent.h"

#include "ArticleTree.generated.h"

class UStaticMeshComponent;

UCLASS()
class PLANET_API AArticleTree : public ARawMaterialBase
{
	GENERATED_BODY()

public:

	AArticleTree(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

protected:

private:

};
