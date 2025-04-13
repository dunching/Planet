// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "GroupManaggerInterface.h"
#include "PlanetAIController.h"

#include "TeamMatesHelperComponent.h"

#include "HumanAIController.generated.h"

class USplineComponent;
class UCharacterTitle;
class UTeamMatesHelperComponent;
struct FCharacterProxy;
class ACharacterBase;
class AHumanCharacter_AI;
class UStateTreeComponent;
class UAIControllerStateTreeAIComponent;
class UAIPerceptionComponent;
class ABuildingArea;
class AGeneratorColony;
class AGeneratorNPCs_Patrol;
class AGroupManagger;

/**
 *
 */
UCLASS()
class PLANET_API AHumanAIController :
	public APlanetAIController
{
	GENERATED_BODY()

public:

	using FCharacterProxyType = FCharacterProxy;

	using FPawnType = AHumanCharacter_AI;

	using FTeamHelperChangedDelegate =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	using FTeammateOptionChangedDelegate = 
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr < FCharacterProxyType>&)>::FCallbackHandleSPtr;

	AHumanAIController(const FObjectInitializer& ObjectInitializer);

	virtual void SetGroupSharedInfo(AGroupManagger*GroupManaggerPtr) override;

	virtual UAIPerceptionComponent* GetAIPerceptionComponent();

	virtual TObjectPtr<UAIControllerStateTreeAIComponent> GetStateTreeAIComponent()const;

	virtual bool CheckIsFarawayOriginal() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<AGeneratorNPCs_Patrol> GeneratorNPCs_PatrolPtr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> PathFollowComponentPtr = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<ABuildingArea> BuildingAreaPtr = nullptr;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnTeammateOptionChangedImp(
		ETeammateOption TeammateOption,
		const TSharedPtr < FCharacterProxyType>& LeaderCharacterProxyPtr
	);

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn)override;

	virtual void OnUnPossess() override;

	virtual void OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr) override;

	void OnGroupChanged();

	void OnTeamChanged();

	void InitialCharacter();

	void InitialAIConony();

	void InitialSenseConfig();

	void InitialAllocations();

	FTeammateOptionChangedDelegate TeammateOptionChangedDelegate;

	FTeamHelperChangedDelegate TeamHelperChangedDelegate;

	FTeamHelperChangedDelegate GroupHelperChangedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TObjectPtr<UAIControllerStateTreeAIComponent> StateTreeAIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponentPtr = nullptr;

};
