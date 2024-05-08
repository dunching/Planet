// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterBase.h"
#include <SceneElement.h>
#include "EquipmentElementComponent.h"

#include "HumanCharacter.generated.h"

class UInputComponent;
class UGravitySpringComponent;
class UCameraComponent;
class FProcessActionBase;
class USceneObjPropertyComponent;
class FInputProcessor;
class UAnimInstanceCharacter;
class UZYInputComponent;
class UAIHumanInfo;
class UGroupMnaggerComponent;
class UGourpMateUnit;

UCLASS()
class PLANET_API AHumanCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:

	AHumanCharacter(const FObjectInitializer& ObjectInitializer);

	virtual TPair<FVector, FVector>GetCharacterViewInfo();

	UGourpMateUnit* GetGourpMateUnit();

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	TSoftObjectPtr<UTexture2D> CharacterIcon;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ToolsIcons")
	UGourpMateUnit* GourpMateUnitPtr = nullptr;

private:

};
