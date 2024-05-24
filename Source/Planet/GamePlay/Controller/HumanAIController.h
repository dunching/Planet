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
class UStateTreeComponent;
class UStateTreeAIComponent;

/**
 *
 */
UCLASS()
class PLANET_API AHumanAIController : public AGravityAIController, public IHumanControllerInterface
{
	GENERATED_BODY()

public:

	using FTeamOptionChangedDelegate =
		UGroupsManaggerSubSystem::FTeammateOptionChangedDelegateContainer::FCallbackHandleSPtr;

	using FTeamHelperChangedDelegate =
		UGroupMnaggerComponent::FTeamHelperChangedDelegateContainer::FCallbackHandleSPtr;

	AHumanAIController(const FObjectInitializer& ObjectInitializer);

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;

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

	virtual void OnUnPossess() override;

	void OnGroupChanged();

	void OnTeamChanged();

	void InitialCharacter();

	FTeamOptionChangedDelegate TeammateOptionChangedDelegateContainer;

	FTeamHelperChangedDelegate TeamHelperChangedDelegate;

	FTeamHelperChangedDelegate GroupHelperChangedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UStateTreeComponent> StateTreeComponentPtr = nullptr;

};