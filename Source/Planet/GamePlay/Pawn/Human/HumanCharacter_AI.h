// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanCharacter.h"
#include "GenerateType.h"

#include "HumanCharacter_AI.generated.h"

class UAIComponent;

UCLASS()
class PLANET_API AHumanCharacter_NPC : public AHumanCharacter
{
	GENERATED_BODY()

public:

	AHumanCharacter_NPC(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ETeammateOption DefaultTeammateOption = ETeammateOption::kEnemy;
#endif

protected:
	
	UPROPERTY()
	TObjectPtr<UAIComponent> AIComponentPtr = nullptr;
	
};

UCLASS()
class PLANET_API AHumanCharacter_AI : public AHumanCharacter_NPC
{
	GENERATED_BODY()

public:

};
