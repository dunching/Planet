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

	UCameraComponent* GetCameraComp();;

	USpringArmComponent* GetCameraBoom();;

	virtual TPair<FVector, FVector>GetCharacterViewInfo();

protected:
	
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;
	
	virtual void OnRep_GroupSharedInfoChanged()override;
	
	virtual void InitialGroupSharedInfo();

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
