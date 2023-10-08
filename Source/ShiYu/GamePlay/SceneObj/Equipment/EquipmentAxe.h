// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EquipmentBase.h"
#include "Pawn/PawnDataStruct.h"

#include "EquipmentAxe.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;

UCLASS(BlueprintType, Blueprintable)
class SHIYU_API UEquipmentAxeInteractionComponent : public UEquipmentInteractionComponent
{
	GENERATED_BODY()

public:

};

UCLASS()
class SHIYU_API AEquipmentAxe : public AEquipmentBase
{
	GENERATED_BODY()

public:

	AEquipmentAxe(const FObjectInitializer& ObjectInitializer);

	virtual void EquipItemToCharacter(AHumanCharacter* CharacterPtr)override;

	virtual void DoAction(EEquipmentActionType ActionType)override;

protected:

	virtual void OnAttackAnimtionStart();

	virtual void OnAttackAnimtionStop();

	virtual void StartAttack();

	virtual void EndAttack();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Simulation Fist")
		USceneComponent* SceneCompPtr = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USkeletalMeshComponent* MeshComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
		UNiagaraComponent* NiagaraAxePtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackType")
		bool bWhetherToKeepAttacking = false;

private:

	bool bIsAttacking = false;

};
