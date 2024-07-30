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

class AEquipmentBase;
class ASceneObj;
class UCharacterTitle;

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
class UInteractiveBaseGAComponent;
class UInteractiveConsumablesComponent;
class UInteractiveSkillComponent;
class UInteractiveToolComponent;

UCLASS()
class PLANET_API ACharacterBase : 
	public AGravityCharacter,
	public IPlanetAbilitySystemInterface,
	public ISceneObjInteractionInterface
{
	GENERATED_BODY()

public:

	using FTeamMembersChangedDelegateHandle = TCallbackHandleContainer<void(EGroupMateChangeType, ACharacterBase*)>::FCallbackHandleSPtr;

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual ~ACharacterBase();

	virtual void PossessedBy(AController* NewController) override;

	virtual void Interaction(ACharacterBase* CharacterPtr) override;

	virtual void StartLookAt(ACharacterBase* CharacterPtr) override;

	virtual void EndLookAt() override;

	UFUNCTION(BlueprintPure, Category = "Character")
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UHoldingItemsComponent* GetHoldingItemsComponent();

	UCharacterAttributesComponent* GetCharacterAttributesComponent();

	UTalentAllocationComponent* GetTalentAllocationComponent();

	UInteractiveBaseGAComponent* GetInteractiveBaseGAComponent();

	UInteractiveConsumablesComponent* GetInteractiveConsumablesComponent();

	UInteractiveSkillComponent* GetInteractiveSkillComponent();

	UInteractiveToolComponent* GetInteractiveToolComponent();

	UGroupMnaggerComponent* GetGroupMnaggerComponent();

	template<typename Type = UAnimInstanceBase>
	Type* GetAnimationIns();

	virtual bool IsGroupmate(ACharacterBase*TargetCharacterPtr)const;

	virtual bool IsTeammate(ACharacterBase* TargetCharacterPtr)const;

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchAnimLink(EAnimLinkClassType AnimLinkClassType);

	UCharacterTitle* CharacterTitlePtr = nullptr;

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void OnCharacterGroupMateChanged(
		EGroupMateChangeType GroupMateChangeType,
		ACharacterBase* LeaderPCPtr
	);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
	float BaseTurnRate = 45.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
	float BaseLookUpRate = 45.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TObjectPtr<UPlanetAbilitySystemComponent> AbilitySystemComponentPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
	TObjectPtr<UCharacterAttributesComponent> CharacterAttributesComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UTalentAllocationComponent> TalentAllocationComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UStateProcessorComponent> StateProcessorComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<UInteractiveBaseGAComponent> InteractiveBaseGAComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<UInteractiveConsumablesComponent> InteractiveConsumablesComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<UInteractiveSkillComponent> InteractiveSkillComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<UInteractiveToolComponent> InteractiveToolComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UGroupMnaggerComponent> GroupMnaggerComponentPtr = nullptr;

	FTeamMembersChangedDelegateHandle TeamMembersChangedDelegateHandle;

private:

	void OnHPChanged(int32 CurrentValue);

	void OnMoveSpeedChanged(int32 CurrentValue);

	FValueChangedDelegateHandle HPChangedHandle;

	FValueChangedDelegateHandle MoveSpeedChangedHandle;

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
