// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"

#include "SceneActor.h"
#include "Common/GenerateType.h"

#include "ToolProxyBase.generated.h"

class UNiagaraComponent;

class AHumanCharacter;
class USkeletalMesh;
class UClass;
class UToolFuture_Base;
class USkill_Base;

static FName RootComponentName;

UCLASS()
class PLANET_API AToolProxyBase : public ASceneActor
{
	GENERATED_BODY()

public:
	using FOwnerPawnType = ACharacterBase;

	AToolProxyBase(
		const FObjectInitializer& ObjectInitializer
		);

protected:
	virtual void BeginPlay() override;

	virtual void AttachToCharacter(
		ACharacterBase* CharacterPtr
		);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Equiment")
	USceneComponent* SceneCompPtr = nullptr;

	static FName RootComponentName;

private:

	UFUNCTION()
	void OnOwnerDestroyde(
		AActor* DestroyedActor
		);
};
