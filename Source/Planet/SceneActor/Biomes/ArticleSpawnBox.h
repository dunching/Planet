// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "ArticleSpawnBox.generated.h"

class UBoxComponent;
class ARawMaterialBase;

UCLASS()
class PLANET_API AArticleSpawnBox : public AActor
{
	GENERATED_BODY()

public:

	AArticleSpawnBox();

protected:

	virtual void BeginPlay()override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "StaticMesh")
		UBoxComponent* BoxCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TArray<TSubclassOf<ARawMaterialBase>>NeedRespawnActorAry;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StaticMesh")
        bool bIsReSpawn= true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "StaticMesh")
		float SpawnDensity = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StaticMesh")
		int32 Distance = 100;

private:

	void ResetSpawnItem();

	void SpawnItem();

	FTimerHandle SpawnActorTimer;

};
