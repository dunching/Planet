// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "HumanControllerInterface.h"
#include "GroupsManaggerSubSystem.h"
#include "GroupMnaggerComponent.h"

#include "HumanAIController.generated.h"

class UAIHumanInfo;
class UGroupMnaggerComponent;
class UGourpmateUnit;
class ACharacterBase;
class AHumanCharacter;

/**
 *
 */
UCLASS()
class PLANET_API AHumanAIController : public AGravityAIController, public IHumanControllerInterface
{
	GENERATED_BODY()

public:

	using FTeammateOptionChangedDelegateContainer =
		UGroupsManaggerSubSystem::FTeammateOptionChangedDelegateContainer::FCallbackHandleSPtr;

	using FTeamHelperChangedDelegateContainer =
		UGroupMnaggerComponent::FTeamHelperChangedDelegateContainer::FCallbackHandleSPtr;

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;

	virtual FPawnType* GetCharacter()override;

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
		FPawnType* LeaderPCPtr
	);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn)override;

	void OnTeamHelperChanged();

	FTeammateOptionChangedDelegateContainer TeammateOptionChangedDelegateContainer;

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

};