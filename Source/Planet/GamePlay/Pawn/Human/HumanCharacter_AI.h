// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterAttributesComponent.h"
#include "HumanCharacter.h"
#include "GenerateTypes.h"
#include "SceneActorInteractionComponent.h"
#include "StateProcessorComponent.h"

#include "HumanCharacter_AI.generated.h"

class UNiagaraComponent;
class UAIComponent;
class AGuideInteraction_HumanCharacter_AI;
class UResourceBoxStateTreeComponent;
class AGroupManagger_NPC;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API USceneCharacterAIInteractionComponent : public USceneActorInteractionComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_AI;

	virtual void BeginPlay() override;
	
	virtual void StartInteractionItem(
		const TSubclassOf<AQuestInteractionBase>& Item
		) override;
	
	virtual void ChangedInterationState(
		const TSubclassOf<AQuestInteractionBase>& Item,
		bool bIsEnable
	) override;

private:
	
	void UpdatePromt()const;
};

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UCharacterAIAttributesComponent : public UCharacterAttributesComponent
{
	GENERATED_BODY()

public:
	virtual void SetCharacterID(
		const FGuid& InCharacterID
	) override;
};

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UCharacterNPCStateProcessorComponent : public UStateProcessorComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = AHumanCharacter_AI;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	virtual TArray<TWeakObjectPtr<ACharacterBase>> GetTargetCharactersAry() const override;

	void SetTargetCharactersAry(
		const TWeakObjectPtr<ACharacterBase>& TargetCharacter
	);

private:
	UPROPERTY(Replicated)
	TWeakObjectPtr<ACharacterBase> TargetCharacter = nullptr;
};

UCLASS()
class PLANET_API AHumanCharacter_AI :
	public AHumanCharacter
{
	GENERATED_BODY()

public:
	AHumanCharacter_AI(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void BeginPlay() override;

	virtual void PossessedBy(
		AController* NewController
	) override;

	virtual void SpawnDefaultController() override;

	UCharacterNPCStateProcessorComponent* GetCharacterNPCStateProcessorComponent() const;

	AGroupManagger_NPC* GetGroupManagger_NPC() const;

	TObjectPtr<UNiagaraComponent>  GetNiagaraComponent() const;

	virtual void HasBeenStartedLookAt(
		ACharacterBase* InCharacterPtr
	) override;

	virtual void HasBeenLookingAt(
		ACharacterBase* InCharacterPtr
	) override;

	virtual void HasBeenEndedLookAt() override;

	void SetGroupSharedInfo(
		AGroupManagger* GroupManaggerPtr
	);

	virtual void OnPlayerGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	void SetCharacterID(
		const FGuid& InCharacterID
	);

	UAIComponent* GetAIComponent() const;

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	virtual void OnRep_GroupManagger() override;

	// virtual TSharedPtr<FCharacterProxy> GetCharacterProxy()const override;

	virtual void OnSelfGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
	) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TObjectPtr<UAIComponent> AIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TObjectPtr<UNiagaraComponent> NiagaraComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	UWidgetComponent* InteractionWidgetCompoentPtr = nullptr;
};
