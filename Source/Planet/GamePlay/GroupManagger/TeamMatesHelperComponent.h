// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenerateType.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"
#include "TeamConfigure.h"

#include "TeamMatesHelperComponent.generated.h"

class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
class AGroupManagger;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
class UPlanetAbilitySystemComponent;
class UProxySycHelperComponent;
struct FSceneProxyContainer;

/*
 *	“小队”信息
 *
 *	共享GroupManagger数据的Character会记录在此
 *	玩家的队伍配置会记录在此
 */
UCLASS(BlueprintType, Blueprintable)
class UTeamMatesHelperComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	using FCharacterProxyType = FCharacterProxy;

	static FName ComponentName;

	using FPawnType = ACharacterBase;

	using FCharacterProxyType = FCharacterProxy;

	using FOwnerType = AGroupManagger;

	using FMemberChangedDelegateContainer =
	TCallbackHandleContainer<void(
		const FTeammate&,
		const TSharedPtr<FCharacterProxyType>&


	
		)>;

	using FTeammateOptionChangedDelegateContainer =
	TCallbackHandleContainer<void(
		ETeammateOption,
		const TSharedPtr<FCharacterProxyType>&


	
		)>;

	using FKnowCharaterChanged =
	TCallbackHandleContainer<void(
		TWeakObjectPtr<ACharacterBase>,
		bool
		)>;

	using FOnFocusCharacterDelegate =
	TCallbackHandleContainer<void(
		ACharacterBase*


	
		)>;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	UTeamMatesHelperComponent(
		const FObjectInitializer& ObjectInitializer
		);

	void SwitchTeammateOption(
		ETeammateOption InTeammateOption
		);

	ETeammateOption GetTeammateOption() const;

#pragma region 锁定,感知到的目标
	void AddKnowCharacter(
		ACharacterBase* CharacterPtr
		);

	void RemoveKnowCharacter(
		ACharacterBase* CharacterPtr
		);

	void SetFocusCharactersAry(
		ACharacterBase* TargetCharacterPtr
		);

	void ClearFocusCharactersAry();

	TWeakObjectPtr<ACharacterBase> GetForceKnowCharater() const;

	TSet<TWeakObjectPtr<ACharacterBase>> GetSensingChractersSet() const;

	void SetSensingChractersSet(
		const TSet<TWeakObjectPtr<ACharacterBase>>& KnowCharater
		);

#pragma endregion

	void SpwanTeammateCharacter();

	void UpdateTeammateConfig(
		const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr,
		int32 Index
		);

	bool IsMember(
		const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr
		) const;

	bool IsMember(
		const FGuid& CharacterID
		) const;

	bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest = false,
		bool bNoCheck = false
		);

	TSharedPtr<FCharacterProxyType> GetOwnerCharacterProxy() const;

	void SetOwnerCharacterProxy(
		const TSharedPtr<FCharacterProxyType>& CharacterProxySPtr
		);
	
	TSet<TSharedPtr<FCharacterProxyType>>GetMembersSet() const;
	
	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

	/**
	 * 
	 */
	FMemberChangedDelegateContainer MembersChanged;

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	TSet<AHumanCharacter*> TargetSet;

	FOnFocusCharacterDelegate OnFocusCharacterDelegate;

protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

private:
	/**
	 * 更新感知到的目标
	 * 移除无效的感知到的目标
	 */
	void UpdateSensingCharacters();

	bool CheckCharacterIsValid(
		const TWeakObjectPtr<ACharacterBase>& CharacterPtr
		);

#pragma region RPC

public:
	/**
	 * 
	 */
	UFUNCTION(Server, Reliable)
	virtual void TeammateCharacter_ActiveWeapon_Server();

private:
	UFUNCTION(Server, Reliable)
	void SwitchTeammateOption_Server(
		ETeammateOption InTeammateOption
		);

	UFUNCTION(Server, Reliable)
	virtual void UpdateTeammateConfig_Server(
		const FGuid& ProxtID,
		int32 Index
		);

	/**
	 * 生成AI队友
	 */
	UFUNCTION(Server, Reliable)
	virtual void SpwanTeammateCharacter_Server();

#pragma endregion

	UFUNCTION()
	void OnRep_GroupSharedInfoChanged();

	UFUNCTION()
	void OnRep_TeammateOptionChanged();

	void OnAddToNewTeam(
		const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr
		);

	void UpdateTeammateConfigImp(
		FPawnType* PCPtr,
		int32 Index
		);

	void UpdateTeammateConfigImp(
		const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr,
		int32 Index
		);

	TSharedPtr<FCharacterProxyType> OwnerCharacterProxyPtr = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_TeammateOptionChanged)
	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

	/**
	 * 感知到的目标
	 */
	TSet<TWeakObjectPtr<ACharacterBase>> SensingChractersSet;

	/**
	* 玩家锁定的目标
	* 这个角色锁定的目标，第0个为主要锁定
	* 为什么不用Controller UpdateRotation去做？因为我们要在移动组件里统一设置旋转
	*/
	UPROPERTY(Replicated)
	TWeakObjectPtr<ACharacterBase> ForceKnowCharater;

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	FTeamConfigure TeamConfigure;

	FTimerHandle CheckKnowCharacterTimerHandle;

	/**
	 * 组内Character信息
	 * 包括玩家生成的AI队友和召唤物
	 */
	TSet<TSharedPtr<FCharacterProxyType>> MembersSet;

	UPROPERTY(Replicated)
	TArray<FGuid> MembersIDSet;
};
