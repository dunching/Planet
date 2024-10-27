// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanCharacter.h"

#include "HumanCharacter_Player.generated.h"

class UPlayerComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class PLANET_API AHumanCharacter_Player : public AHumanCharacter
{
	GENERATED_BODY()

public:

	AHumanCharacter_Player(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UCameraComponent* GetCameraComp();;

	USpringArmComponent* GetCameraBoom();;

	virtual TPair<FVector, FVector>GetCharacterViewInfo();

protected:
	
#if WITH_EDITORONLY_DATA

#endif
	
	/** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	/** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;

	UPROPERTY()
	TObjectPtr<UPlayerComponent> PlayerComponentPtr = nullptr;

};
