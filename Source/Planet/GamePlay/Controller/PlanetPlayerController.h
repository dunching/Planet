// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include <AIController.h>

#include "GravityPlayerController.h"
#include "GenerateType.h"
#include "HumanControllerInterface.h"

#include "PlanetPlayerController.generated.h"

class ACharacterBase;
class IPlanetControllerInterface;
class UGourpmateUnit;
class UFocusIcon;

/**
 *
 */
UCLASS()
class PLANET_API APlanetPlayerController : public AGravityPlayerController, public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	APlanetPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	AActor* GetFocusActor() const;

	virtual void ClearFocus(EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	FVector GetFocalPoint() const;

	FVector GetFocalPointOnActor(const AActor* Actor) const;

protected:

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime)override;

	virtual void UpdateRotation(float DeltaTime)override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void SetPawn(APawn* InPawn) override;

	virtual bool InputKey(const FInputKeyParams& Params)override;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;

	void InitialCharacter();

	UFUNCTION()
	void OnFocusEndplay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	void OnFocusDeathing(const FGameplayTag Tag, int32 Count);

	void BindRemove(AActor* Actor);

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	FFocusKnowledge	FocusInformation;

	UFocusIcon* FocusIconPtr = nullptr;

};