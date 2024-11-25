// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GravityAIController.h"

#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UPlanetAbilitySystemComponent;
struct FCharacterProxy;
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

	virtual AGroupSharedInfo* GetGroupSharedInfo() const override;

	virtual UHoldingItemsComponent* GetHoldingItemsComponent()const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const override;

	virtual TSharedPtr<FCharacterProxy> GetCharacterUnit() override;

	virtual ACharacterBase* GetRealCharacter()const override;
	
	virtual TWeakObjectPtr<ACharacterBase> GetTeamFocusTarget() const;
	
	virtual bool CheckIsFarawayOriginal() const;

	virtual bool IsGroupmate(ACharacterBase* TargetCharacterPtr)const;

	virtual bool IsTeammate(ACharacterBase* TargetCharacterPtr)const;

	void OnHPChanged(int32 CurrentValue);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn)override;

	virtual void ResetGroupmateUnit(FCharacterProxy* NewGourpMateUnitPtr)override;

	virtual void BindPCWithCharacter()override;

	virtual TSharedPtr<FCharacterProxy> InitialCharacterUnit(ACharacterBase* CharaterPtr)override;
	
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:

	void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax);

};
