// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include <AIController.h>

#include "GravityPlayerController.h"
#include "GroupManaggerInterface.h"
#include "HumanCharacter_AI.h"
#include "PlanetControllerInterface.h"
#include "OnEffectedTargetCallback.h"

#include "PlanetPlayerController.generated.h"

class ACharacterBase;
class AHumanCharacter_Player;
class AHumanCharacter_AI;
class IPlanetControllerInterface;
struct FCharacterProxy;
class UFocusIcon;
class UCharacterAttributesComponent;
class UInventoryComponent;
class UTalentAllocationComponent;
class UTeamMatesHelperComponent;
class UEventSubjectComponent;
class AGroupManagger;
class AGeneratorColony_ByTime;
class AGeneratorBase;
class AGeneratorColony_ByInvoke;
class AGuideActor;
class ATeleport;
class UPlayerControllerGameplayTasksComponent;

/**
 *
 */
UCLASS()
class PLANET_API APlanetPlayerController :
	public AGravityPlayerController,
	public IPlanetControllerInterface,
	public IGroupManaggerInterface
{
	GENERATED_BODY()

public:
	using FPawnType = AHumanCharacter_Player;

	APlanetPlayerController(
		const FObjectInitializer& ObjectInitializer
		);

	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual AGroupManagger* GetGroupManagger() const override;

	virtual void SetGroupSharedInfo(
		AGroupManagger* GroupManaggerPtr
		) override;

	virtual UInventoryComponent* GetInventoryComponent() const override;

	virtual UCharacterAttributesComponent* GetCharacterAttributesComponent() const override;

	virtual UTalentAllocationComponent* GetTalentAllocationComponent() const override;

	virtual TWeakObjectPtr<ACharacterBase> GetTeamFocusTarget() const;

	virtual TSharedPtr<FCharacterProxy> GetCharacterProxy() override;

	virtual ACharacterBase* GetRealCharacter() const override;

	void OnHPChanged(
		int32 CurrentValue
		);

	UEventSubjectComponent* GetEventSubjectComponent() const;

	TObjectPtr<UPlayerControllerGameplayTasksComponent> GetGameplayTasksComponent() const;

	void AddProvideEyesViewActor(
		const TObjectPtr<AActor>& ProvideEyesViewActor
		);

	void RemoveProvideEyesViewActor(
		const TObjectPtr<AActor>& ProvideEyesViewActor
		);

#pragma region CMD

	UFUNCTION(Server, Reliable)
	void EntryChallengeLevel(
		ETeleport Teleport
		);

	UFUNCTION(Server, Reliable)
	void MakeTrueDamege(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void MakeTrueDamegeInArea(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void MakeTherapy(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void ReplyStamina(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void MakeRespawn(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void AddShield(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void AddShieldToTarget(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void ModifyElementalData(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void ModifyElementalDataToTarget(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void SetCharacterAttributeValue(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void SetCharacterAttributeTemporaryValue(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void RepelTarget(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void StunTarget(
		const TArray<FString>& Args
		);

#pragma endregion

#pragma region RPC

	UFUNCTION(Server, Reliable)
	void ServerSpawnGeneratorActor(
		const TSoftObjectPtr<AGeneratorColony_ByInvoke>& GeneratorBasePtr
		);

	UFUNCTION(Server, Reliable)
	void ServerSpawnCharacterAry(
		const TArray<TSubclassOf<AHumanCharacter_AI>>& CharacterClassAry,
		const TArray<FGuid>& IDAry,
		const TArray<FTransform>& TransformAry,
		ETeammateOption TeammateOption
		);

	UFUNCTION(Server, Reliable)
	void ServerSpawnCharacter(
		TSubclassOf<AHumanCharacter_AI> CharacterClass,
		const FGuid& ID,
		const FTransform& Transform,
		ETeammateOption TeammateOption
		);

	UFUNCTION(Server, Reliable)
	void ServerSpawnCharacterByProxyType(
		const FGameplayTag& CharacterProxyType,
		const FTransform& Transform
		);

	UFUNCTION(Server, Reliable)
	void ServerDestroyActor(
		AActor* ActorPtr
		);

	UFUNCTION(Server, Reliable)
	void BuyProxys(
		ACharacterBase* InTraderCharacterPtr,
		const FGameplayTag& BuyProxyTag,
		const FGuid& BuyProxyID,
		int32 Num,
		const FGameplayTag& CostProxyType,
		int32 Cost
		);

	UFUNCTION(Server, Reliable)
	void IncreaseCD(
		int32 CD
		);

	UFUNCTION(Server, Reliable)
	void AddProxy(
		const FGameplayTag& ProxyType,
		int32 Num
		);

	UFUNCTION(Server, Reliable)
	void RemoveProxy(
		const FGameplayTag& ProxyType
		);

	UFUNCTION(Server, Reliable)
	void SwitchPlayerInput(
		const TArray<FString>& Args
		);

	UFUNCTION(Server, Reliable)
	void AdjustTime(
		const FDateTime& Time
		);

	UFUNCTION(Server, Reliable)
	void UpdateWeather(
		const FGameplayTag& WeatherType
		);

	UFUNCTION(Server, Reliable)
	void AddOrRemoveState(
		const FGameplayTag& StateTag,
		bool bIsAdd
		);

	UFUNCTION(NetMulticast, Reliable)
	void AddOrRemoveStateImp(
		const FGameplayTag& StateTag,
		bool bIsAdd
		);

	UFUNCTION(Server, Reliable)
	void AddExperience(
		int32 ExperienceValue
		);

	/**
	 * 更新角色的天赋分配
	 * @param CharacterID 
	 * @param TalentSocket 
	 * @param NewLevel 
	 */
	UFUNCTION(Server, Reliable)
	void UpdateCharacterTalent(
		const FGuid& CharacterID,
		const FGameplayTag& TalentSocket,
		int32 NewLevel
		);

	/**
	 * 完成之后 调用 OnUpgradeSkillComplete，其他情况暂时不考虑
	 * @param SkillID 
	 * @param CosumeProxysIDS 
	 * @param CosumeProxyNums 
	 */
	UFUNCTION(Server, Reliable)
	void UpgradeSkill(
		const FGuid& SkillID,
		const TArray<FGuid>& CosumeProxysIDS,
		const TArray<uint32>& CosumeProxyNums
		);

	/**
	 * 将新增的词条传到Client
	 * 为什么不通过属性同步？
	 * 因为我们现在的结构不支持
	 * @param GeneratedPropertyEntryInfoAry 
	 */
	UFUNCTION(Client, Reliable)
	void OnUpgradeSkillComplete(
		const TArray<FGeneratedPropertyEntryInfo>& GeneratedPropertyEntryInfoAry
		);

	using FOnUpgradeSkillComplete = TCallbackHandleContainer<void(const TArray<FGeneratedPropertyEntryInfo>&)>;

	FOnUpgradeSkillComplete OnUpgradeSkillCompleteDelegate;

#pragma endregion

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
		) override;

	virtual void PlayerTick(
		float DeltaTime
		) override;

	virtual void OnPossess(
		APawn* InPawn
		) override;

	virtual void OnUnPossess() override;

	virtual void SetPawn(
		APawn* InPawn
		) override;

	virtual bool InputKey(
		const FInputKeyEventArgs& Params
		) override;

	virtual void GetActorEyesViewPoint(
		FVector& Location,
		FRotator& Rotation
		) const override;

	virtual void OnRep_PlayerState() override;

	virtual void OnGroupManaggerReady(
		AGroupManagger* NewGroupSharedInfoPtr
		) override;

	virtual void ResetGroupmateProxy(
		FCharacterProxy* NewGourpMateProxyPtr
		) override;

	virtual void BindPCWithCharacter() override;

	virtual TSharedPtr<FCharacterProxy> InitialCharacterProxy(
		ACharacterBase* CharaterPtr
		) override;

	// 初始化共享信息相关的内容
	virtual void InitialGroupSharedInfo();

	UFUNCTION()
	void OnRep_GroupManagger();

	UFUNCTION()
	void OnRep_WolrdProcess();

	// 给被锁定的目标绑定一些回调，比如目标进入“死亡”、“隐身”、“不可选中”时
	void BindOnFocusRemove(
		AActor* Actor
		);

	void OnPossessCharacterMakedDamage(
		const FOnEffectedTargetCallback& OnEffectedTargetCallback
		);

	using FMakedDamageDelegate = TCallbackHandleContainer<void(
		const FOnEffectedTargetCallback&


		
		)>;

	FMakedDamageDelegate::FCallbackHandleSPtr MakedDamageDelegateHandle;

	UPROPERTY(ReplicatedUsing = OnRep_GroupManagger)
	TObjectPtr<AGroupManagger> GroupManaggerPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UEventSubjectComponent> EventSubjectComponentPtr = nullptr;

	UPROPERTY()
	TObjectPtr<UPlayerControllerGameplayTasksComponent> GameplayTasksComponentPtr = nullptr;

	FDelegateHandle OnOwnedDeathTagDelegateHandle;

	FFocusKnowledge FocusInformation;

	TSet<TWeakObjectPtr<AActor>> ProvideEyesViewActorSet;
};
