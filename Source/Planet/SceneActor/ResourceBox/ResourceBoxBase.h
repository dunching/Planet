// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SceneActor.h"

#include "ResourceBoxBase.generated.h"

class UInventoryComponent;
class ACharacterBase;

UCLASS()
class PLANET_API AResourceBoxBase : public ASceneActor
{
	GENERATED_BODY()

public:

	AResourceBoxBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void HasbeenInteracted(ACharacterBase* CharacterPtr)override;

protected:
	
	UFUNCTION(NetMulticast, Reliable)
	void InteractionImp_BoxBase(ACharacterBase* CharacterPtr);

	void AddItemsToTarget();

	ACharacterBase* TargetCharacterPtr = nullptr;

#if WITH_EDITORONLY_DATA
	
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, int32> ProxyMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundWave>OpenBoxSoundRef = nullptr;
	
	bool bIsOpend = false;

};
