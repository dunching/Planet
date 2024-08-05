// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"

#include "SceneObj.h"
#include "Common/GenerateType.h"

#include "ToolUnitBase.generated.h"

class UNiagaraComponent;

class AHumanCharacter;
class USkeletalMesh;
class UClass;
class UToolFuture_Base;
class USkill_Base;

UCLASS()
class PLANET_API AToolUnitBase : public ASceneObj
{
	GENERATED_BODY()

public:

	AToolUnitBase(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void AttachToCharacter(AHumanCharacter* CharacterPtr);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equiment")
	USceneComponent* SceneCompPtr = nullptr;

	virtual void BeginPlay()override;

private:

};
