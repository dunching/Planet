// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>

#include "GravityAIController.h"
#include "GroupManaggerInterface.h"
#include "PlanetControllerInterface.h"

#include "PlanetAIController.generated.h"

class UPlanetAbilitySystemComponent;
struct FCharacterProxy;
class ACharacterBase;
class UCharacterAttributesComponent;
class UInventoryComponent;
class UTalentAllocationComponent;
class UTeamMatesHelperComponent;
class AGroupManagger;
class USplineComponent;


UCLASS()
class PLANET_API APlanetAIController : 
	public AGravityAIController, 
	public IPlanetControllerInterface,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:

	using FPawnType = ACharacterBase;

	APlanetAIController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn)override;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual AGroupManagger* GetGroupManagger() const override;

	virtual void SetGroupSharedInfo(AGroupManagger*GroupManaggerPtr) override;

	virtual UInventoryComponent* GetHoldingItemsComponent()const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent()const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent()const override;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy() override;

	virtual ACharacterBase* GetRealCharacter()const override;
	
	virtual TWeakObjectPtr<ACharacterBase> GetTeamFocusTarget() const;
	
	virtual bool CheckIsFarawayOriginal() const;

	virtual bool IsGroupmate(ACharacterBase* TargetCharacterPtr)const;

	virtual bool IsTeammate(ACharacterBase* TargetCharacterPtr)const;

	void OnHPChanged(int32 CurrentValue);

	virtual FPathFollowingRequestResult MoveAlongSPline(
		USplineComponent* SplineComponentPtr,
		const FAIMoveRequest& MoveRequest,
		FNavPathSharedPtr* OutPath = nullptr);

protected:

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ResetGroupmateProxy(FCharacterProxy* NewGourpMateProxyPtr)override;

	virtual void BindPCWithCharacter()override;

	virtual TSharedPtr<FCharacterProxy> InitialCharacterProxy(ACharacterBase* CharaterPtr)override;
	
	virtual void OnGroupManaggerReady(AGroupManagger* NewGroupSharedInfoPtr) override;

	UFUNCTION()
	void OnRep_GroupSharedInfoChanged();

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	TObjectPtr<AGroupManagger> GroupManaggerPtr = nullptr;

private:

	void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax);

};