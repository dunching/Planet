// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Weapon_Base.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Weapon_Bow.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API AWeapon_Bow : public AWeapon_Base
{
	GENERATED_BODY()

public:

	AWeapon_Bow(const FObjectInitializer& ObjectInitializer);

	virtual void AttachToCharacter(ACharacterBase* CharacterPtr)override;

	USkeletalMeshComponent* GetMesh();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName Socket;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* SkeletalComponentPtr = nullptr;

};
