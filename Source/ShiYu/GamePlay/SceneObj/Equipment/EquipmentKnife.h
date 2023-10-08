// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "Pawn/PawnDataStruct.h"
#include "EquipmentKnife.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;

UCLASS(BlueprintType, Blueprintable)
class SHIYU_API UEquipmentKnifeInteractionComponent  : public UEquipmentInteractionComponent
{
	GENERATED_BODY()

public:

	UEquipmentKnifeInteractionComponent(const FObjectInitializer& ObjectInitializer);

private:

};

UCLASS()
class SHIYU_API AEquipmentKnife : public AEquipmentBase
{
	GENERATED_BODY()

public:

	AEquipmentKnife(const FObjectInitializer& ObjectInitializer);

	virtual void EquipItemToCharacter(AHumanCharacter* CharacterPtr)override;

	virtual void OnAttackAnimtionStart();

	virtual void OnAttackAnimtionStop();

protected:

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USkeletalMeshComponent* KnifeSkeletalMeshCompPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
		UNiagaraComponent* Knife_SystemPtr = nullptr;

private:

};
