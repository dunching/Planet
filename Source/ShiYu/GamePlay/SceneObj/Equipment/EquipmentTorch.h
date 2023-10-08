// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EquipmentBase.h"
#include "Pawn/PawnDataStruct.h"
#include "EquipmentTorch.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;

UCLASS()
class SHIYU_API AEquipmentTorch : public AEquipmentBase
{
	GENERATED_BODY()

public:

	AEquipmentTorch(const FObjectInitializer& ObjectInitializer);

	virtual void EquipItemToCharacter(AHumanCharacter* CharacterPtr)override;

protected:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
        USceneComponent* SceneCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* TorchStaticMeshCompPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
		UNiagaraComponent* Knife_SystemPtr = nullptr;

private:

};
