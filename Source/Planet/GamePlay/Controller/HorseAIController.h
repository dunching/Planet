// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "PlanetAIController.h"

#include "GroupMnaggerComponent.h"

#include "HorseAIController.generated.h"

class UCharacterTitle;
class UGroupMnaggerComponent;
struct FCharacterProxy;
class ACharacterBase;
class AHorseCharacter;
class AHumanCharacter;
class UStateTreeComponent;
class UStateTreeAIComponent;
class UAIPerceptionComponent;

/**
 *
 */
UCLASS()
class PLANET_API AHorseAIController : public APlanetAIController
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = FCharacterProxy;

	using FPawnType = AHorseCharacter;

	using FTeammateOptionChangedDelegate =
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterUnitType>&)>::FCallbackHandleSPtr;

	using FTeamHelperChangedDelegate =
		TCallbackHandleContainer<void()>::FCallbackHandleSPtr;

	AHorseAIController(const FObjectInitializer& ObjectInitializer);

	virtual void SetGroupSharedInfo(AGroupSharedInfo*GroupSharedInfoPtr) override;

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UAIPerceptionComponent* GetAIPerceptionComponent() ;

	UCharacterTitle* AIHumanInfoPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TWeakObjectPtr<AHumanCharacter> TargetCharacterPtr;

protected:

	void OnTeammateOptionChangedImp(
		ETeammateOption TeammateOption,
		const TSharedPtr<FCharacterUnitType>& LeaderPCPtr
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponentPtr = nullptr;

};
