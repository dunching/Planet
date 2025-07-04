// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"
#include "OpenWorldDataLayer.h"

#include "PlanetPlayerState.generated.h"

class UPlanetAbilitySystemComponent;
class UProxySycHelperComponent;
class UAudioComponent;
struct FCharacterProxy;
struct FSceneProxyContainer;

/*
 *	玩家数据
 */
UCLASS()
class PLANET_API APlanetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	using FOnRegionChanged =
	TCallbackHandleContainer<void(const FGameplayTag&)>;

	APlanetPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	bool GetIsInChallenge()const;

#pragma region RPC
	UFUNCTION(Server, Reliable)
	void SetEntryChanlleng(bool bIsEntryChanlleng);
#pragma endregion

	FGameplayTag GetRegionTag()const;

	void SetCurrentTeleport(ETeleport InCurrentTeleport);
	
	FOnRegionChanged OnRegionChanged;
	
	/*
	 *	角色进入副本之前在开放世界的位置信息
	 */
	FTransform OpenWorldTransform = FTransform::Identity;
	
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
		) const override;

	void InitialData();

private:

	void UpdatePosition();

	void UpdateCurrentPosition(const FGameplayTag&NewCurrentRegionTag);
	
	UFUNCTION()
	void OnRep_RegionTag();
	
	UFUNCTION()
	void OnRep_CurrentTeleport();
	
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent>AudioComponentPtr = nullptr;
	
	FString PlayerName;

	UPROPERTY(ReplicatedUsing = OnRep_RegionTag)
	FGameplayTag CurrentRegionTag;

	UPROPERTY(Replicated)
	bool bIsInChallenge = false;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTeleport)
	ETeleport CurrentTeleport = ETeleport::kNone;
	
};
