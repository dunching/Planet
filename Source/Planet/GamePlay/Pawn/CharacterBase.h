// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "Character/GravityCharacter.h"

#include "GroupManaggerInterface.h"
#include "PlanetAbilitySystemComponent.h"
#include "SceneActorInteractionInterface.h"
#include "TemplateHelper.h"

#include "CharacterBase.generated.h"

class UAbilitySystemComponent;

struct FGameplayAbilityTargetData_GAReceivedEvent;

class AEquipmentBase;
class ASceneActor;
class UCharacterTitle;
class UCharacterRisingTips;

class USkeletalMeshComponent;
class UAnimInstanceBase;
class UEquipmentInteractionComponent;
class UHoldItemComponent;
class UInputProcessorSubSystem;
class UZYInputComponent;
class USceneActorPropertyComponent;
class UPlanetGameplayAbility;
class UCharacterAttributesComponent;
class UInventoryComponent;
class UPlanetAbilitySystemComponent;
class UTalentAllocationComponent;
class UStateProcessorComponent;
class UTeamMatesHelperComponent;
class UCharacterAbilitySystemComponent;
class UInteractiveConsumablesComponent;
class USceneActorInteractionComponent;
class UCharacterTitleComponent;
class UProxyProcessComponent;
class UInteractiveToolComponent;
class UConversationComponent;
class UWidgetComponent;
class AGroupManagger;

struct FCharacterProxy;

UCLASS()
class PLANET_API ACharacterBase :
	public AGravityCharacter,
	public IPlanetAbilitySystemInterface,
	public IGroupManaggerInterface,
	public ISceneActorInteractionInterface
{
	GENERATED_BODY()

public:
	using FCharacterProxyType = FCharacterProxy;

	using FValueChangedDelegateHandle =
	TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	using FProcessedGAEventHandle =
	TCallbackHandleContainer<void(
		const FGameplayAbilityTargetData_GAReceivedEvent&
		
		)>::FCallbackHandleSPtr;

	using FOnInitaliedGroupSharedInfo =
	TCallbackHandleContainer<void()>;

	ACharacterBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual ~ACharacterBase();

	virtual void PossessedBy(
		AController* NewController
		) override;

	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;

	virtual void SetActorHiddenInGame(
		bool bNewHidden
		) override;

	UFUNCTION(BlueprintPure, Category = "Character")
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/**
	 * ROLE_AutonomousProxy的数据准备好时
	 */
	UFUNCTION()
	virtual void OnRep_GroupSharedInfoChanged();

	AGroupManagger* GetGroupManagger() const;

	UInventoryComponent* GetInventoryComponent() const;

	UCharacterAttributesComponent* GetCharacterAttributesComponent() const;

	UTalentAllocationComponent* GetTalentAllocationComponent() const;

	UCharacterAbilitySystemComponent* GetCharacterAbilitySystemComponent() const;

	UStateProcessorComponent* GetStateProcessorComponent() const;

	UProxyProcessComponent* GetProxyProcessComponent() const;

	UConversationComponent* GetConversationComponent() const;

	UCharacterTitleComponent* GetCharacterTitleComponent() const;

	virtual USceneActorInteractionComponent* GetSceneActorInteractionComponent() const override;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy() const;

	USkeletalMeshComponent* GetCopyPoseMesh() const;

	template <typename Type = UAnimInstanceBase>
	Type* GetAnimationIns();

	virtual bool IsGroupmate(
		ACharacterBase* TargetCharacterPtr
		) const;

	virtual bool IsTeammate(
		ACharacterBase* TargetCharacterPtr
		) const;

	TObjectPtr<ACharacterBase> GetFocusActor() const;

	UFUNCTION(NetMulticast, Reliable)
	void SwitchAnimLink_Client(
		EAnimLinkClassType AnimLinkClassType
		);

	UFUNCTION(NetMulticast, Reliable)
	void SetCampType(
		ECharacterCampType CharacterCampType
		);

	// 确认是否是一个有效的选中目标，比如目标在隐身、或“无法选中”、重伤倒地状态时为不可已被选中
	bool GetIsValidTarget() const;

	FOnInitaliedGroupSharedInfo OnInitaliedGroupSharedInfo;

protected:
	virtual void OnConstruction(
		const FTransform& Transform
		) override;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
		) override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	virtual void SpawnDefaultController() override;

	virtual void OnGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	virtual void HasbeenInteracted(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenLookingAt(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenStartedLookAt(
		ACharacterBase* CharacterPtr
		) override;

	virtual void HasBeenEndedLookAt() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchAnimLink(
		EAnimLinkClassType AnimLinkClassType
		);

	UPROPERTY(Category=Character, EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> CopyPoseMeshPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UCharacterAttributesComponent> CharacterAttributesComponentPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UTalentAllocationComponent> TalentAllocationComponentPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UStateProcessorComponent> StateProcessorComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TObjectPtr<UCharacterAbilitySystemComponent> AbilitySystemComponentPtr = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Data)
	TObjectPtr<UCharacterTitleComponent> CharacterTitleComponentPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UProxyProcessComponent> ProxyProcessComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ConversationBorder)
	TObjectPtr<UConversationComponent> ConversationComponentPtr = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	TObjectPtr<AGroupManagger> GroupManaggerPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WolrdProcess)
	TObjectPtr<USceneActorInteractionComponent> SceneActorInteractionComponentPtr = nullptr;

private:
	void OnHPChanged(
		const FOnAttributeChangeData& CurrentValue
		);

	void OnMoveSpeedChanged(
		const FOnAttributeChangeData& CurrentValue
		);

	void OnMoveSpeedChangedImp(
		float Value
		);

	void OnDeathing(
		const FGameplayTag Tag,
		int32 Count
		);

	void DoDeathing();

	FValueChangedDelegateHandle HPChangedHandle;

	FValueChangedDelegateHandle MoveSpeedChangedHandle;

	FProcessedGAEventHandle ProcessedGAEventHandle;

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	UPROPERTY(Transient)
	TObjectPtr<AController> OriginalAIController;
};

template <typename Type>
Type* ACharacterBase::GetAnimationIns()
{
	auto CurSkeletalMeshCompPtr = GetMesh();
	if (CurSkeletalMeshCompPtr)
	{
		return Cast<Type>(CurSkeletalMeshCompPtr->GetAnimInstance());
	}

	return nullptr;
}
