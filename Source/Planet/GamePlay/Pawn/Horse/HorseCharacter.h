// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterBase.h"
#include <SceneElement.h>
#include "EquipmentElementComponent.h"

#include "HorseCharacter.generated.h"

class UInputComponent;
class UWidgetComponent;
class UCameraComponent;

class UGravitySpringComponent;

class FProcessActionBase;
class USceneObjPropertyComponent;
class FInputProcessor;
class UAnimInstanceCharacter;

UCLASS()
class PLANET_API AHorseCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:

	AHorseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual TPair<FVector, FVector>GetCharacterViewInfo();

	void SwitchDisplayMountTips(bool bIsDisplay);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Camera)
	float MaxLookDown = 30.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Camera)
	float MaxLookUp = 80.f;

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UWidgetComponent* MountTipsWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UCapsuleComponent* LeftMountCheckVolumetric = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UCapsuleComponent* RightMountCheckVolumetric = nullptr;

private:

};