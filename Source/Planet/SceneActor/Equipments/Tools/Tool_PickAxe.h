// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Tool_Base.h"
#include "PlanetGameplayAbility.h"
#include "Planet_Tool_Base.h"
#include "Skill_Base.h"

#include "Tool_PickAxe.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API ATool_PickAxe : public APlanet_Tool_Base
{
	GENERATED_BODY()

public:

	ATool_PickAxe(const FObjectInitializer& ObjectInitializer);

	virtual void DoActionByCharacter(AHumanCharacter* CharacterPtr, EEquipmentActionType ActionType);

	virtual void AttachToCharacter(ACharacter* CharacterPtr)override;

	USkeletalMeshComponent* GetMesh();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName Socket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* SkeletalComponentPtr = nullptr;

};
