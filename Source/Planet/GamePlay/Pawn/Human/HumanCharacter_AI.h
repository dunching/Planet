// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanCharacter.h"
#include "GenerateType.h"

#include "HumanCharacter_AI.generated.h"

class UAIComponent;

UCLASS()
class PLANET_API AHumanCharacter_AI : public AHumanCharacter
{
	GENERATED_BODY()

public:

	AHumanCharacter_AI(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	void SetGroupSharedInfo(AGroupSharedInfo*GroupSharedInfoPtr);

	void SetCharacterID(const FGuid& InCharacterID);
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ETeammateOption DefaultTeammateOption = ETeammateOption::kEnemy;
#endif
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_Allocation_RowName = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag AI_CharacterType = FGameplayTag::EmptyTag;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_GroupSharedInfoChanged()override;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy()const override;
	
	UFUNCTION()
	void OnRep_CharacterID();
	
	void InitialAllocationsRowName();

	void InitialAllocationsByProxy();

	UPROPERTY()
	TObjectPtr<UAIComponent> AIComponentPtr = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterID)
	FGuid CharacterID;
	
};
