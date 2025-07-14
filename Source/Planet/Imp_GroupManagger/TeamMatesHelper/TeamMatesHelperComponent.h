// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ItemProxy_Minimal.h"
#include "TeamConfigure.h"
#include "TeamMatesHelperComponentBase.h"

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
class PLANET_API UTeamMatesHelperComponent : public UTeamMatesHelperComponentBase
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

	virtual void SwitchTeammateOption(
		ETeammateOption InTeammateOption
		) override;

#pragma endregion

	virtual void SpwanTeammateCharacter() override;

	void UpdateTeammateConfig(
		const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr,
		int32 Index
		);

	bool IsMember(
		const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr
		) const;

	virtual bool IsMember(
		const FGuid& CharacterID
		) const override;

	virtual bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest = false,
		bool bNoCheck = false
		) override;

	TSharedPtr<FCharacterProxyType> GetOwnerCharacterProxy() const;

	void SetOwnerCharacterProxy(
		const TSharedPtr<FCharacterProxyType>& CharacterProxySPtr
		);

	TSet<TSharedPtr<FCharacterProxyType>> GetMembersSet() const;

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
	virtual void OnRep_MembersIDSet() override;

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

private:
	
	UFUNCTION()
	void OnRep_GroupManagger();

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

	FTimerHandle CheckKnowCharacterTimerHandle;

	/**
	 * 组内Character信息
	 * 包括玩家生成的AI队友和召唤物
	 */
	TSet<TSharedPtr<FCharacterProxyType>> MembersSet;
};
