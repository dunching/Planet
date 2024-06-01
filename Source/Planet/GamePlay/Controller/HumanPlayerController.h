// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include <AIController.h>

#include "GravityPlayerController.h"
#include "GenerateType.h"
#include "HumanControllerInterface.h"

#include "HumanPlayerController.generated.h"

class ACharacterBase;
class IPlanetControllerInterface;
class UGourpmateUnit;
class UFocusIcon;

/**
 *
 */
UCLASS()
class PLANET_API AHumanPlayerController : public AGravityPlayerController, public IHumanControllerInterface
{
	GENERATED_BODY()

public:

	using FTeamMembersChangedDelegateHandle = TCallbackHandleContainer<void(EGroupMateChangeType, FPawnType*)>::FCallbackHandleSPtr;

	AHumanPlayerController(const FObjectInitializer& ObjectInitializer);

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

	void OnCharacterGroupMateChanged(
		EGroupMateChangeType GroupMateChangeType,
		FPawnType* LeaderPCPtr
	);

	UFUNCTION()
	void OnFocusEndplay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	void OnFocusDeathing(const FGameplayTag Tag, int32 Count);

	void BindRemove(AActor* Actor);

	FTeamMembersChangedDelegateHandle TeamMembersChangedDelegateHandle;

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	FFocusKnowledge	FocusInformation;

	UFocusIcon* FocusIconPtr = nullptr;

};
