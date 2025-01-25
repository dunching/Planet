// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include <AIController.h>

#include "GravityPlayerController.h"
#include "GenerateType.h"
#include "GroupSharedInterface.h"
#include "PlanetControllerInterface.h"

#include "PlanetPlayerController.generated.h"

class ACharacterBase;
class AHumanCharacter_Player;
class IPlanetControllerInterface;
struct FCharacterProxy;
class UFocusIcon;
class UCharacterAttributesComponent;
class UInventoryComponent;
class UTalentAllocationComponent;
class UTeamMatesHelperComponent;
class UEventSubjectComponent;
class AGroupSharedInfo;
class AGuideActor;

/**
 *
 */
UCLASS()
class PLANET_API APlanetPlayerController :
	public AGravityPlayerController,
	public IPlanetControllerInterface,
	public IGroupSharedInterface
{
	GENERATED_BODY()

public:
	using FPawnType = AHumanCharacter_Player;

	using FOnFocusCharacterDelegate =
	TCallbackHandleContainer<void(ACharacterBase*)>;

	APlanetPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	AActor* GetFocusActor() const;

	virtual void ClearFocus(EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);

	FVector GetFocalPoint() const;

	FVector GetFocalPointOnActor(const AActor* Actor) const;

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual AGroupSharedInfo* GetGroupSharedInfo() const override;

	virtual void SetGroupSharedInfo(AGroupSharedInfo* GroupSharedInfoPtr) override;

	virtual UInventoryComponent* GetHoldingItemsComponent() const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent() const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent() const override;
	
	virtual TWeakObjectPtr<ACharacterBase> GetTeamFocusTarget() const;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy() override;

	virtual ACharacterBase* GetRealCharacter() const override;

	void OnHPChanged(int32 CurrentValue);

	UEventSubjectComponent * GetEventSubjectComponent()const;
	
	FOnFocusCharacterDelegate OnFocusCharacterDelegate;

#pragma region CMD

	UFUNCTION(Server, Reliable)
	void MakeTrueDamege(const TArray<FString>& Args);

	UFUNCTION(Server, Reliable)
	void MakeTherapy(const TArray<FString>& Args);

	UFUNCTION(Server, Reliable)
	void MakeRespawn(const TArray<FString>& Args);

#pragma endregion

protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PlayerTick(float DeltaTime) override;

	virtual void UpdateRotation(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void SetPawn(APawn* InPawn) override;

	virtual bool InputKey(const FInputKeyParams& Params) override;

	virtual void OnRep_PlayerState()override;
	
	virtual void OnGroupSharedInfoReady(AGroupSharedInfo* NewGroupSharedInfoPtr) override;

	virtual void ResetGroupmateProxy(FCharacterProxy* NewGourpMateProxyPtr) override;

	virtual void BindPCWithCharacter() override;

	virtual TSharedPtr<FCharacterProxy> InitialCharacterProxy(ACharacterBase* CharaterPtr) override;

	// 初始化共享信息相关的内容
	virtual void InitialGroupSharedInfo();

	UFUNCTION()
	void OnRep_GroupSharedInfoChanged();

	UFUNCTION()
	void OnRep_WolrdProcess();

	UFUNCTION()
	void OnFocusEndplay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	// 目标进入死亡状态
	void OnFocusDeathing(const FGameplayTag Tag, int32 Count);

	// 给被锁定的目标绑定一些回调，比如目标进入“死亡”、“隐身”、“不可选中”时
	void BindOnFocusRemove(AActor* Actor);

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	TObjectPtr<AGroupSharedInfo> GroupSharedInfoPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UEventSubjectComponent> EventSubjectComponentPtr = nullptr;
	
	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	FFocusKnowledge FocusInformation;
	
};
