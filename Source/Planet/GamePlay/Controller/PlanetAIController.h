// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GravityAIController.h"
#include "GroupSharedInterface.h"
#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UPlanetAbilitySystemComponent;
struct FCharacterProxy;
class ACharacterBase;
class UCharacterAttributesComponent;
class UHoldingItemsComponent;
class UTalentAllocationComponent;
class UTeamMatesHelperComponent;
class AGroupSharedInfo;


UCLASS()
class PLANET_API APlanetAIController : 
	public AGravityAIController, 
	public IPlanetControllerInterface,
	public IGroupSharedInterface
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	APlanetAIController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn)override;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual AGroupSharedInfo* GetGroupSharedInfo() const override;

	virtual void SetGroupSharedInfo(AGroupSharedInfo*GroupSharedInfoPtr) override;

	virtual UHoldingItemsComponent* GetHoldingItemsComponent()const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const override;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy() override;

	virtual ACharacterBase* GetRealCharacter()const override;
	
	virtual TWeakObjectPtr<ACharacterBase> GetTeamFocusTarget() const;
	
	virtual bool CheckIsFarawayOriginal() const;

	virtual bool IsGroupmate(ACharacterBase* TargetCharacterPtr)const;

	virtual bool IsTeammate(ACharacterBase* TargetCharacterPtr)const;

	void OnHPChanged(int32 CurrentValue);

protected:

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn)override;

	virtual void ResetGroupmateProxy(FCharacterProxy* NewGourpMateProxyPtr)override;

	virtual void BindPCWithCharacter()override;

	virtual TSharedPtr<FCharacterProxy> InitialCharacterProxy(ACharacterBase* CharaterPtr)override;
	
	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	virtual void InitialGroupSharedInfo();

	UFUNCTION()
	void OnRep_GroupSharedInfoChanged();

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	TObjectPtr<AGroupSharedInfo> GroupSharedInfoPtr = nullptr;

private:

	void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax);

};