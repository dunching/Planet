// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include <AIController.h>

#include "GravityPlayerController.h"
#include "GenerateType.h"
#include "PlanetControllerInterface.h"

#include "PlanetPlayerController.generated.h"

class ACharacterBase;
class IPlanetControllerInterface;
class UCharacterUnit;
class UFocusIcon;
class UCharacterAttributesComponent;
class UHoldingItemsComponent;
class UTalentAllocationComponent;
class UGroupMnaggerComponent;

/**
 *
 */
UCLASS()
class PLANET_API APlanetPlayerController : public AGravityPlayerController, public IPlanetControllerInterface
{
	GENERATED_BODY()

public:

	using FPawnType = AHumanCharacter;

	APlanetPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	AActor* GetFocusActor() const;

	virtual void ClearFocus(EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	FVector GetFocalPoint() const;

	FVector GetFocalPointOnActor(const AActor* Actor) const;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UGroupMnaggerComponent* GetGroupMnaggerComponent() const override;

	virtual UHoldingItemsComponent* GetHoldingItemsComponent()const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const override;

	virtual UCharacterUnit* GetCharacterUnit() override;

	virtual ACharacterBase* GetRealCharacter()const override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PlayerTick(float DeltaTime)override;

	virtual void UpdateRotation(float DeltaTime)override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void SetPawn(APawn* InPawn) override;

	virtual bool InputKey(const FInputKeyParams& Params)override;

	virtual void ResetGroupmateUnit(UCharacterUnit* NewGourpMateUnitPtr)override;

	virtual void BindPCWithCharacter()override;

	virtual UCharacterUnit* InitialCharacterUnit(ACharacterBase * CharaterPtr)override;

	UFUNCTION()
	void OnFocusEndplay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	void OnFocusDeathing(const FGameplayTag Tag, int32 Count);

	void BindRemove(AActor* Actor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	TObjectPtr<UCharacterAttributesComponent> CharacterAttributesComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UTalentAllocationComponent> TalentAllocationComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	FFocusKnowledge	FocusInformation;

	UFocusIcon* FocusIconPtr = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterUnit> CharacterUnitPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RowName")
	FGameplayTag RowName = FGameplayTag::EmptyTag;

	// 
	TObjectPtr<FPawnType> RealCharacter;

};
