// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Weapon_Base.h"
#include "PlanetGameplayAbility.h"
#include "PlanetWeapon_Base.h"
#include "Skill_Base.h"
#include "ProjectileBase.h"

#include "Weapon_FoldingFan.generated.h"

class AStaticMeshActor;
class ASkeletalMeshActor;
class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class PLANET_API AWeapon_FoldingFan : public APlanetWeapon_Base
{
	GENERATED_BODY()

public:

	AWeapon_FoldingFan(const FObjectInitializer& ObjectInitializer);

	virtual void AttachToCharacter(ACharacter* CharacterPtr)override;

	USkeletalMeshComponent* GetMesh()const;
	
	UFUNCTION(NetMulticast, Reliable)
	void BeginAttack();
	
	UFUNCTION(NetMulticast, Reliable)
	void EndAttack();
	
	USphereComponent* GetCollisionComponent();

	bool bIsReachFarestPoint = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementCompPtr = nullptr;
	
protected:

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void Tick(float DeltaSeconds)override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast, Reliable)
	void OnReachFarestPoint();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Socket;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalComponentPtr = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Niagara")
	UNiagaraComponent* NiagaraComponent = nullptr;

	int32 MaxMoveRange = -1;

	FVector StartPt = FVector::ZeroVector;

};
