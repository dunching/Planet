// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InputActions.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 *
 */
UCLASS()
class PLANET_API UInputActions : public UObject
{
	GENERATED_BODY()
public:

	void InitialInputMapping();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveForwardActionPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveRightActionPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		TMap<FKey, UInputAction*>KeyEventMap;

};
