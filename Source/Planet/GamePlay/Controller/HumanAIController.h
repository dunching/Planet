// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "PlanetAIController.h"
#include "GroupsManaggerSubSystem.h"
#include "GroupMnaggerComponent.h"

#include "HumanAIController.generated.h"

class USplineComponent;
class UCharacterTitle;
class UGroupMnaggerComponent;
class UCharacterUnit;
class ACharacterBase;
class AHumanCharacter;
class UStateTreeComponent;
class UStateTreeAIComponent;
class UAIPerceptionComponent;
class ABuildingArea;

/**
 *
 */
UCLASS()
class PLANET_API AHumanAIController : public APlanetAIController
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = UCharacterUnit;

	using FPawnType = AHumanCharacter;

	using FTeamHelperChangedDelegate =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	using FTeammateOptionChangedDelegate = 
		TCallbackHandleContainer<void(ETeammateOption, FCharacterUnitType*)>::FCallbackHandleSPtr;

	AHumanAIController(const FObjectInitializer& ObjectInitializer);

	void InitialSenseConfig();

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UAIPerceptionComponent* GetAIPerceptionComponent();

	virtual bool CheckIsFarawayOriginal() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USplineComponent> PatrolSPlinePtr = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<ABuildingArea> BuildingAreaPtr = nullptr;

protected:

	void OnTeammateOptionChangedImp(
		ETeammateOption TeammateOption,
		FCharacterUnitType* LeaderCharacterUnitPtr
	);

	void OnDeathing(const FGameplayTag Tag, int32 Count);

	void DoDeathing();

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPossess(APawn* InPawn)override;

	virtual void OnUnPossess() override;

	void OnGroupChanged();

	void OnTeamChanged();

	void InitialCharacter();

	FTeammateOptionChangedDelegate TeammateOptionChangedDelegate;

	FTeamHelperChangedDelegate TeamHelperChangedDelegate;

	FTeamHelperChangedDelegate GroupHelperChangedDelegate;

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponentPtr = nullptr;

};
