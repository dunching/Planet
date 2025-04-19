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
		TCallbackHandleContainer<void(EGroupMateChangeType, const TSharedPtr<FCharacterProxyType>&)>;

	using FTeammateOptionChangedDelegateContainer = 
		TCallbackHandleContainer<void(ETeammateOption, const TSharedPtr<FCharacterProxyType>&)>;

	using FKnowCharaterChanged = 
		TCallbackHandleContainer<void(TWeakObjectPtr<ACharacterBase>, bool)>;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	UTeamMatesHelperComponent(const FObjectInitializer& ObjectInitializer);

	void SwitchTeammateOption(ETeammateOption InTeammateOption);

	ETeammateOption GetTeammateOption()const;

	void AddKnowCharacter(ACharacterBase*CharacterPtr);

	void RemoveKnowCharacter(ACharacterBase* CharacterPtr);

	void SpwanTeammateCharacter();

	void UpdateTeammateConfig(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr, int32 Index);

	bool IsMember(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr)const;

	bool IsMember(const FGuid& CharacterID)const;

	bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest=false,
		bool bNoCheck=false
		);
	
	TWeakObjectPtr<ACharacterBase> GetKnowCharacter()const;
	
	TSharedPtr<FCharacterProxyType>GetOwnerCharacterProxyPtr()const;
	
	void SetOwnerCharacterProxy(const TSharedPtr<FCharacterProxyType>&CharacterProxySPtr);
	
	FTeammateOptionChangedDelegateContainer TeammateOptionChanged;

	FKnowCharaterChanged KnowCharaterChanged;

	FMemberChangedDelegateContainer MembersChanged;

	// 分配的小队
	TSet<TSharedPtr<FCharacterProxyType>> MembersSet;

	TWeakObjectPtr<ACharacterBase>ForceKnowCharater;

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	TSet<AHumanCharacter*>TargetSet;

protected:

	virtual void InitializeComponent()override;

	virtual void BeginPlay()override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	virtual void UpdateTeammateConfig_Server(const FGuid&ProxtID, int32 Index);

	UFUNCTION(Server, Reliable)
	virtual void SpwanTeammateCharacter_Server();

private:
	
	UFUNCTION()
	void OnRep_GroupSharedInfoChanged();

	UFUNCTION()
	void OnRep_TeammateOptionChanged();

	void OnAddToNewTeam(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr);

	void UpdateTeammateConfigImp(FPawnType* PCPtr, int32 Index);

	void UpdateTeammateConfigImp(const TSharedPtr<FCharacterProxyType>& CharacterProxyPtr, int32 Index);

	void CheckKnowCharacterImp();
	
	TSharedPtr<FCharacterProxyType> OwnerCharacterProxyPtr = nullptr;

	FTimerHandle CheckKnowCharacterTimerHandle;
	
	UPROPERTY(ReplicatedUsing = OnRep_TeammateOptionChanged)
	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

	TArray<TPair<TWeakObjectPtr<ACharacterBase>, int32>>KnowCharatersSet;

	UPROPERTY(ReplicatedUsing = OnRep_GroupSharedInfoChanged)
	FTeamConfigure TeamConfigure;

};
