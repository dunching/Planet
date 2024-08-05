// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Weapon_Base.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Weapon_PickAxe.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API AWeapon_PickAxe : public AWeapon_Base
{
	GENERATED_BODY()

public:

	AWeapon_PickAxe(const FObjectInitializer& ObjectInitializer);

	virtual void AttachToCharacter(AHumanCharacter* CharacterPtr)override;

	USkeletalMeshComponent* GetMesh();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName Socket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* SkeletalComponentPtr = nullptr;

};
