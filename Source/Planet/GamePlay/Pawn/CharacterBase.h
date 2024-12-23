// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "Character/GravityCharacter.h"

#include "SceneElement.h"
#include "PlanetAbilitySystemComponent.h"
#include "SceneObjInteractionInterface.h"

#include "CharacterBase.generated.h"

class UAbilitySystemComponent;

struct FGameplayAbilityTargetData_GAReceivedEvent;

class AEquipmentBase;
class ASceneObj;
class UCharacterTitle;
class UCharacterRisingTips;

class UAnimInstanceBase;
class UEquipmentInteractionComponent;
class UHoldItemComponent;
class UInputProcessorSubSystem;
class UZYInputComponent;
class USceneObjPropertyComponent;
class UPlanetGameplayAbility;
class UCharacterAttributesComponent;
class UHoldingItemsComponent;
class UPlanetAbilitySystemInterface;
class UTalentAllocationComponent;
class UStateProcessorComponent;
class UGroupMnaggerComponent;
class UBaseFeatureComponent;
class UInteractiveConsumablesComponent;
class UProxyProcessComponent;
class UInteractiveToolComponent;
class UCDCaculatorComponent;
class UWidgetComponent;

UCLASS()
class PLANET_API ACharacterBase : 
	public AGravityCharacter,
	public IPlanetAbilitySystemInterface,
	public ISceneObjInteractionInterface
{
	GENERATED_BODY()

public:

	using FCharacterUnitType = FCharacterProxy;

	using FTeamMembersChangedDelegateHandle = 
		TCallbackHandleContainer<void(EGroupMateChangeType, const TSharedPtr<FCharacterUnitType>&)>::FCallbackHandleSPtr;

	using FValueChangedDelegateHandle =
		TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	using FProcessedGAEventHandle = 
		TCallbackHandleContainer<void(const FGameplayAbilityTargetData_GAReceivedEvent&)>::FCallbackHandleSPtr;

	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual ~ACharacterBase();

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;
	
	virtual void InteractionSceneObj(ASceneObj* SceneObjPtr);
	
	virtual void Interaction(ACharacterBase* CharacterPtr) override;

	virtual void LookingAt(ACharacterBase* CharacterPtr)override;

	virtual void StartLookAt(ACharacterBase* CharacterPtr) override;

	virtual void EndLookAt() override;

	UFUNCTION(BlueprintPure, Category = "Character")
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UHoldingItemsComponent* GetHoldingItemsComponent()const;

	UCharacterAttributesComponent* GetCharacterAttributesComponent()const;

	UTalentAllocationComponent* GetTalentAllocationComponent()const;

	UBaseFeatureComponent* GetBaseFeatureComponent()const;
	
	UStateProcessorComponent* GetStateProcessorComponent()const;

	UProxyProcessComponent* GetProxyProcessComponent()const;

	UGroupMnaggerComponent* GetGroupMnaggerComponent()const;
	
	UCDCaculatorComponent* GetCDCaculatorComponent()const;

	TSharedPtr<FCharacterProxy> GetCharacterUnit()const;
	
	template<typename Type = UAnimInstanceBase>
	Type* GetAnimationIns();

	virtual bool IsGroupmate(ACharacterBase*TargetCharacterPtr)const;

	virtual bool IsTeammate(ACharacterBase* TargetCharacterPtr)const;
	
	UFUNCTION(NetMulticast, Reliable)
	void SwitchAnimLink_Client(EAnimLinkClassType AnimLinkClassType);
	
	UFUNCTION(NetMulticast, Reliable)
	void SetCampType(ECharacterCampType CharacterCampType);

 	UPROPERTY(Transient)
 	UCharacterTitle* CharacterTitlePtr = nullptr;
 	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "RowName")
	FGameplayTag RowName = FGameplayTag::EmptyTag;
	
protected:

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void SpawnDefaultController()override;

	void InitialDefaultCharacterUnit();
	
	UFUNCTION(Server, Reliable)
	virtual void InteractionSceneObj_Server(ASceneObj* SceneObjPtr);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchAnimLink(EAnimLinkClassType AnimLinkClassType);
	
	void OnCharacterGroupMateChanged(
		EGroupMateChangeType GroupMateChangeType,
		const TSharedPtr<FCharacterUnitType>& TargetCharacterUnitPtr
	);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
	float BaseTurnRate = 45.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
	float BaseLookUpRate = 45.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<UPlanetAbilitySystemComponent> AbilitySystemComponentPtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	TObjectPtr<UCharacterAttributesComponent> CharacterAttributesComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UTalentAllocationComponent> TalentAllocationComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UStateProcessorComponent> StateProcessorComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UBaseFeatureComponent> BaseFeatureComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UProxyProcessComponent> ProxyProcessComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCDCaculatorComponent> CDCaculatorComponentPtr = nullptr;
	
	FTeamMembersChangedDelegateHandle TeamMembersChangedDelegateHandle;

private:

	UFUNCTION(NetMulticast, Reliable)
	void OnHPChanged(int32 CurrentValue);
	
	UFUNCTION(NetMulticast, Reliable)
	void OnMoveSpeedChanged(int32 CurrentValue);
	
	UFUNCTION(NetMulticast, Unreliable)
	void OnProcessedGAEVent(const FGameplayAbilityTargetData_GAReceivedEvent& GAEvent);

	FValueChangedDelegateHandle HPChangedHandle;

	FValueChangedDelegateHandle MoveSpeedChangedHandle;

	FProcessedGAEventHandle ProcessedGAEventHandle;
	
	UPROPERTY(Transient)
	TObjectPtr<AController> OriginalAIController;

};

template<typename Type>
Type* ACharacterBase::GetAnimationIns()
{
	auto CurSkeletalMeshCompPtr = GetMesh();
	if (CurSkeletalMeshCompPtr)
	{
		return Cast<Type>(CurSkeletalMeshCompPtr->GetAnimInstance());
	}

	return nullptr;
}
