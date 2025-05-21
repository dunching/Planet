// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCameraCommon.h"

#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"

#include "GenerateType.h"

#include "PlayerComponent.generated.h"

struct FInputActionValue;

class USceneComponent;
class UInputActions;
class AHumanCharacter_Player;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UPlayerComponent :
	public UActorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_Player;

	static FName ComponentName;

	UPlayerComponent(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BeginPlay() override;

	virtual bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest = false,
		bool bNoCheck = false
		);

	virtual void PossessedBy(
		APlayerController* NewController
		);

	void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent
		);

	void SetCameraType(
		ECameraType NewCameraType
		);

protected:
	virtual void MoveForward(
		const FInputActionValue& InputActionValue
		);

	virtual void MoveRight(
		const FInputActionValue& InputActionValue
		);

	virtual void AddPitchInput(
		const FInputActionValue& InputActionValue
		);

	virtual void AddYawInput(
		const FInputActionValue& InputActionValue
		);

	UPROPERTY()
	UInputActions* InputActionsPtr = nullptr;

	/**
	 *
	 * 暂时写在这里
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraType")
	ECameraType CameraType = ECameraType::kAction;
};
