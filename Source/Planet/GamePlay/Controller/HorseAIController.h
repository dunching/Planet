// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "GravityAIController.h"
#include "GenerateType.h"
#include "HumanControllerInterface.h"
#include "GroupsManaggerSubSystem.h"
#include "GroupMnaggerComponent.h"

#include "HorseAIController.generated.h"

class UAIHumanInfo;
class UGroupMnaggerComponent;
class UGourpmateUnit;
class ACharacterBase;
class AHumanCharacter;
class UStateTreeComponent;
class UStateTreeAIComponent;
class UAIPerceptionComponent;

/**
 *
 */
UCLASS()
class PLANET_API AHorseAIController : public AGravityAIController, public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	using FTeamOptionChangedDelegate =
		UGroupsManaggerSubSystem::FTeammateOptionChangedDelegateContainer::FCallbackHandleSPtr;

	using FTeamHelperChangedDelegate =
		UGroupMnaggerComponent::FTeamHelperChangedDelegateContainer::FCallbackHandleSPtr;

	AHorseAIController(const FObjectInitializer& ObjectInitializer);

	void SetCampType(ECharacterCampType CharacterCampType);

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UGourpmateUnit* GetGourpMateUnit() override;
	
	virtual UAIPerceptionComponent* GetAIPerceptionComponent() ;

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTeamFocusTarget() const;

	UAIHumanInfo* AIHumanInfoPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TWeakObjectPtr<AHumanCharacter> TargetCharacterPtr;

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

	FTeamOptionChangedDelegate TeammateOptionChangedDelegateContainer;

	FTeamHelperChangedDelegate TeamHelperChangedDelegate;

	FTeamHelperChangedDelegate GroupHelperChangedDelegate;

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponentPtr = nullptr;

};