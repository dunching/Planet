// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_Allocation_RowName = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_CharacterType = FGameplayTag::EmptyTag;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_GroupSharedInfoChanged() override;

	// virtual TSharedPtr<FCharacterProxy> GetCharacterProxy()const override;

	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	UFUNCTION()
	void OnRep_CharacterID();

	void InitialAllocationsRowName();

	void InitialAllocationsByProxy();

	// TODO 移动到UCharacterAttributesComponent下面
	UPROPERTY(ReplicatedUsing = OnRep_CharacterID)
	FGuid CharacterID;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "StateTree")
	TObjectPtr<UAIComponent> AIComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	UWidgetComponent* InteractionWidgetCompoentPtr = nullptr;

};
