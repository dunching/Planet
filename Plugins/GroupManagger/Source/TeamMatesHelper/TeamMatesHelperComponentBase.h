// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "TemplateHelper.h"
#include "TeamConfigure.h"
#include "TeamMates_GenericType.h"

#include "TeamMatesHelperComponentBase.generated.h"

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
class GROUPMANAGGER_API UTeamMatesHelperComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	static FName ComponentName;

	using FOwnerType = AGroupManagger;

	using FTeamHelperChangedDelegateContainer = TCallbackHandleContainer<void()>;

	UTeamMatesHelperComponentBase(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void SwitchTeammateOption(
		ETeammateOption InTeammateOption
		);

	ETeammateOption GetTeammateOption() const;

	virtual void SpwanTeammateCharacter();

	virtual bool IsMember(
		const FGuid& CharacterID
		) const;

	virtual bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest = false,
		bool bNoCheck = false
		);

	FTeamHelperChangedDelegateContainer TeamHelperChangedDelegateContainer;

	TSet<AHumanCharacter*> TargetSet;

protected:
	virtual void InitializeComponent() override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	UFUNCTION()
	virtual void OnRep_MembersIDSet();

	UFUNCTION()
	void OnRep_TeamConfigure();

	UFUNCTION()
	void OnRep_TeammateOptionChanged();

	/**
	 * 组内Character信息
	 * 包括玩家生成的AI队友和召唤物
	 */
	UPROPERTY(ReplicatedUsing = OnRep_MembersIDSet, meta = (AllowPrivateAccess = "true"))
	TArray<FGuid> MembersIDSet;
	
	UPROPERTY(ReplicatedUsing = OnRep_TeammateOptionChanged, meta = (AllowPrivateAccess = "true"))
	ETeammateOption TeammateOption = ETeammateOption::kEnemy;

	UPROPERTY(ReplicatedUsing = OnRep_TeamConfigure, meta = (AllowPrivateAccess = "true"))
	FTeamConfigure TeamConfigure;

	FTimerHandle CheckKnowCharacterTimerHandle;
private:
};
