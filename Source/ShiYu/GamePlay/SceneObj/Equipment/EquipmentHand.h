// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "EquipmentHand.generated.h"

class USphereComponent;
class USceneComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;

UCLASS(BlueprintType, Blueprintable)
class SHIYU_API UEquipmentHandInteractionComponent : public UEquipmentInteractionComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName LeftHandSockName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName RightHandSockName;

protected:

private:

};

UCLASS()
class SHIYU_API AEquipmentHand : public AEquipmentBase
{
	GENERATED_BODY()

public:

	AEquipmentHand(const FObjectInitializer& ObjectInitializer);

	virtual void Destroyed() override;

	virtual void EquipItemToCharacter(AHumanCharacter* CharacterPtr)override;

protected:

	virtual void OnAttackAnimtionStart();

	virtual void OnAttackAnimtionStop();

	virtual void StartAttack();

	virtual void EndAttack();

	UFUNCTION()
		void OnHandOverlayBegin(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
        );

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
		USceneComponent* SceneCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
		USphereComponent* SimulationLFistPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
		USphereComponent* SimulationRFistPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
		UNiagaraComponent* NiagaraLeftHandPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
		UNiagaraComponent* NiagaraRightHandPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
		bool bWhetherToKeepAttacking = false;

private:

	bool bIsAttacking = false;

	FDelegateHandle KeyEventDelegateHandle;

};
