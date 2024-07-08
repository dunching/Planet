// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Tool_Base.h"
#include "PlanetGameplayAbility.h"
#include "Skill_Base.h"

#include "Tool_PickAxe.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class UNiagaraComponent;
class AHumanCharacter;
class ATool_PickAxe;

UCLASS()
class SCENEOBJECTS_API ATool_PickAxe : public ATool_Base
{
	GENERATED_BODY()

public:

	ATool_PickAxe(const FObjectInitializer& ObjectInitializer);

	virtual void DoActionByCharacter(FOnwerType* CharacterPtr, EEquipmentActionType ActionType)override;

	virtual void AttachToCharacter(FOnwerType* CharacterPtr)override;

	USkeletalMeshComponent* GetMesh();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equiment")
	FName Socket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* SkeletalComponentPtr = nullptr;

};
