// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <AIController.h>

#include "GravityPlayerController.h"
#include "GenerateType.h"
#include "PlanetControllerInterface.h"

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

	using FDelegateHandle = TCallbackHandleContainer<void(EGroupMateChangeType, IPlanetControllerInterface*)>::FCallbackHandleSPtr;

	APlanetPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	AActor* GetFocusActor() const;

	virtual void ClearFocus(EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	virtual ACharacterBase* GetCharacter()override;

protected:

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime)override;

	virtual void UpdateRotation(float DeltaTime)override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual bool InputKey(const FInputKeyParams& Params)override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;

	void OnCharacterGroupMateChanged(
		EGroupMateChangeType GroupMateChangeType,
		IPlanetControllerInterface* LeaderPCPtr
	);

	FDelegateHandle DelegateHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

	FFocusKnowledge	FocusInformation;

	UFocusIcon* FocusIconPtr = nullptr;

};
