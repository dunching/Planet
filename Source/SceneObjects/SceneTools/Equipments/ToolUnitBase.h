// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"

#include "SceneObj.h"
#include "Common/GenerateType.h"

#include "ToolUnitBase.generated.h"

class UNiagaraComponent;
class UAbilitySystemComponent;

class AHumanCharacter;
class USkeletalMesh;
class UClass;
class UToolFuture_Base;
class USkill_Base;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UUseToolInterface : public UInterface
{
	GENERATED_BODY()
};


class SCENEOBJECTS_API IUseToolInterface
{
	GENERATED_BODY()

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const = 0;

	virtual USkeletalMeshComponent* GetMesh() const = 0;

protected:

private:

};

UCLASS()
class SCENEOBJECTS_API AToolUnitBase : public ASceneObj
{
	GENERATED_BODY()

public:

	using FOnwerType = IUseToolInterface;

	AToolUnitBase(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void AttachToCharacter(FOnwerType* CharacterPtr);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equiment")
	USceneComponent* SceneCompPtr = nullptr;

	virtual void BeginPlay()override;

private:

};
