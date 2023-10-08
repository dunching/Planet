// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterBase.h"
#include "Pawn/PawnIteractionComponent.h"
#include <ItemType.h>
#include "EquipmentSocksComponent.h"

#include "HorseCharacter.generated.h"

class UInputComponent;
class UGravitySpringComponent;
class UCameraComponent;
class FProcessActionBase;
class USceneObjPropertyComponent;
class FInputProcessor;
class UAnimInstanceCharacter;
class UZYInputComponent;

UCLASS()
class SHIYU_API UHorseEquipmentSocksComponent : public UEquipmentSocksComponent
{
	GENERATED_BODY()

public:

	virtual void UseItem(const FItemNum&Item);

};

UCLASS()
class SHIYU_API AHorseCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:

	AHorseCharacter(const FObjectInitializer& ObjectInitializer);

	UGravitySpringComponent* GetSpringArmComponent() { return SpringArmComponentPtr; }

	UCameraComponent* GetCameraComp();

	virtual TPair<FVector, FVector>GetCharacterViewInfo();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Camera)
		float MaxLookDown = 30.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Camera)
		float MaxLookUp = 80.f;

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
		UGravitySpringComponent* SpringArmComponentPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
		UCameraComponent* CameraComponentPtr = nullptr;

private:

};
