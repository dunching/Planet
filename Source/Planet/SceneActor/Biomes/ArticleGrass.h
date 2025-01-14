// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArticleBase.h"
#include "CoreMinimal.h"
#include "ArticleGrass.generated.h"

class UStaticMeshComponent;

UCLASS()
class PLANET_API AArticleGrass : public ARawMaterialBase
{
	GENERATED_BODY()

public:

	AArticleGrass(const FObjectInitializer& ObjectInitializer);

	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr)override;

protected:

private:

};
