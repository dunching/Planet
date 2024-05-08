// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GravityPlayerController.h"
#include "GenerateType.h"
#include "PlanetControllerInterface.h"

#include "PlanetPlayerController.generated.h"

class ACharacterBase;

/**
 *
 */
UCLASS()
class PLANET_API APlanetPlayerController : public AGravityPlayerController, public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	using FDelegateHandle = TCallbackHandleContainer<void(EGroupMateChangeType, ACharacterBase*)>::FCallbackHandleSPtr;

	APlanetPlayerController(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void BeginPlay() override;

	virtual void PlayerTick(float DeltaTime)override;

	virtual void UpdateRotation(float DeltaTime)override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual bool InputKey(const FInputKeyParams& Params)override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() override;

	void OnCharacterGroupMateChanged(EGroupMateChangeType GroupMateChangeType, ACharacterBase* NewCharacterPtr);

	FDelegateHandle DelegateHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

};
