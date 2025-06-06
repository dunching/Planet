// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterBase.h"
#include "ItemProxy_Minimal.h"
#include "ProxyProcessComponent.h"

#include "HumanCharacter.generated.h"

class UInputComponent;
class UGravitySpringComponent;
class UCameraComponent;
class FProcessActionBase;
class USceneActorPropertyComponent;
class FInputProcessor;
class UAnimInstanceCharacter;
class UZYInputComponent;
class UCharacterTitle;
class UTeamMatesHelperComponent;
class USceneActorInteractionComponent;
struct FCharacterProxy;
struct FSkillProxy;
struct FConsumableProxy;

UCLASS()
class PLANET_API AHumanCharacter :
	public ACharacterBase
{
	GENERATED_BODY()

public:

	AHumanCharacter(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> CharacterIcon;
	
private:

};
