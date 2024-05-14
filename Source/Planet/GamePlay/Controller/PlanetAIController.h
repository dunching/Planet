// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "PlanetControllerInterface.h"
#include "GroupsManaggerSubSystem.h"
#include "GroupMnaggerComponent.h"

#include "PlanetAIController.generated.h"

class UAIHumanInfo;
class UGroupMnaggerComponent;
class UGourpmateUnit;
class ACharacterBase;

/**
 *
 */
UCLASS()
class PLANET_API APlanetAIController : public AGravityAIController, public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	using FTeammateOptionChangedDelegateContainer =
		UGroupsManaggerSubSystem::FTeammateOptionChangedDelegateContainer::FCallbackHandleSPtr;

	using FTeamHelperChangedDelegateContainer =
		UGroupMnaggerComponent::FTeamHelperChangedDelegateContainer::FCallbackHandleSPtr;

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;

	virtual ACharacterBase* GetCharacter()override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTeamFocusEnemy() const;

	UAIHumanInfo* AIHumanInfoPtr = nullptr;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void OnTeammateOptionChanged(
		ETeammateOption TeammateOption,
		ACharacterBase*LeaderCharacterPtr
	);

	void OnTeammateOptionChangedImp(
		ETeammateOption TeammateOption,
		IPlanetControllerInterface* LeaderPCPtr
	);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn)override;

	void OnTeamHelperChanged();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

	FTeammateOptionChangedDelegateContainer TeammateOptionChangedDelegateContainer;

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

};
