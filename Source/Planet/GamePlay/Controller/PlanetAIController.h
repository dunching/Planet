// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UAIHumanInfo;
class UGroupMnaggerComponent;
class UGourpMateUnit;

/**
 * 
 */
UCLASS()
class PLANET_API APlanetAIController : public AGravityAIController, public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() override;

	virtual UGourpMateUnit* GetGourpMateUnit() override;

	UAIHumanInfo* AIHumanInfoPtr = nullptr;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn)override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

};
