// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GravityAIController.h"

#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UPlanetAbilitySystemComponent;
class UCharacterUnit;
class ACharacterBase;
class UCharacterAttributesComponent;
class UHoldingItemsComponent;
class UTalentAllocationComponent;
class UGroupMnaggerComponent;


UCLASS()
class PLANET_API APlanetAIController : 
	public AGravityAIController, 
	public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	APlanetAIController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn)override;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UHoldingItemsComponent* GetHoldingItemsComponent()const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const override;

	virtual UCharacterUnit* GetGourpMateUnit() override;

	virtual ACharacterBase* GetRealCharacter()const override;

protected:

	virtual void BeginPlay() override;

	virtual void ResetGroupmateUnit(UCharacterUnit* NewGourpMateUnitPtr)override;

	virtual void BindPCWithCharacter()override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	TObjectPtr<UCharacterAttributesComponent> CharacterAttributesComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UTalentAllocationComponent> TalentAllocationComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterUnit> CharacterUnitPtr = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RowName")
	FGameplayTag RowName = FGameplayTag::EmptyTag;
	
private:

	// 
	TObjectPtr<FPawnType> RealCharacter;

};
