// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterAttributesComponent.h"
#include "HumanCharacter.h"
#include "GenerateType.h"
#include "SceneActorInteractionComponent.h"

#include "HumanCharacter_AI.generated.h"

class UAIComponent;
class UResourceBoxStateTreeComponent;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API USceneCharacterAIInteractionComponent : public USceneActorInteractionComponent
{
	GENERATED_BODY()

public:
	// virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
};

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UCharacterAIAttributesComponent : public UCharacterAttributesComponent
{
	GENERATED_BODY()

public:

	virtual void SetCharacterID(const FGuid& InCharacterID)override;

};

UCLASS()
class PLANET_API AHumanCharacter_AI :
	public AHumanCharacter
{
	GENERATED_BODY()

public:
	AHumanCharacter_AI(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void HasBeenStartedLookAt(ACharacterBase* InCharacterPtr)override;

	virtual void HasBeenLookingAt(ACharacterBase* InCharacterPtr)override;

	virtual void HasBeenEndedLookAt()override;

	void SetGroupSharedInfo(AGroupSharedInfo* GroupSharedInfoPtr);

	void SetCharacterID(const FGuid& InCharacterID);

	UAIComponent* GetAIComponent() const;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	ETeammateOption DefaultTeammateOption = ETeammateOption::kEnemy;
#endif

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_GroupSharedInfoChanged() override;

	// virtual TSharedPtr<FCharacterProxy> GetCharacterProxy()const override;

	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StateTree")
	TObjectPtr<UAIComponent> AIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UWidgetComponent* InteractionWidgetCompoentPtr = nullptr;

};
