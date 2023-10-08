// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ZYInputComponent.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 *
 */
UCLASS()
class SHIYU_API UZYInputComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UZYInputComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual void InitialInputMapping();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveForwardActionPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveRightActionPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		TMap<FKey, UInputAction*>KeyEventMap;

	static FName ComponentName;

};
