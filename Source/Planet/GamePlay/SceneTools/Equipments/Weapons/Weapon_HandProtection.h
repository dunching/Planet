// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"

#include "Weapon_Base.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Weapon_HandProtection.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API AWeapon_HandProtection : public AWeapon_Base
{
	GENERATED_BODY()

public:

	AWeapon_HandProtection(const FObjectInitializer& ObjectInitializer);

	virtual void AttachToCharacter(AHumanCharacter* CharacterPtr)override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName LeftSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName RightSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* LeftSkeletalComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* RightSkeletalComponentPtr = nullptr;

};
