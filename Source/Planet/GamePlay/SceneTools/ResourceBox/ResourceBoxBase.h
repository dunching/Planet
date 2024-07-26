// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SceneObj.h"

#include "ResourceBoxBase.generated.h"

class UHoldingItemsComponent;
class ACharacterBase;

UCLASS()
class PLANET_API AResourceBoxBase : public ASceneObj
{
	GENERATED_BODY()

public:

	AResourceBoxBase(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Interaction(ACharacterBase* CharacterPtr)override;

protected:

	void AddItemsToTarget();

	ACharacterBase* TargetCharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;
	
#if WITH_EDITORONLY_DATA
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ESkillUnitType, int32> SkillUnitMap;
	
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EConsumableUnitType, int32> ConsumableUnitMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EToolUnitType, int32> ToolUnitMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EWeaponUnitType, int32> WeaponUnitMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECoinUnitType, int32> CoinUnitMap;
	
};
