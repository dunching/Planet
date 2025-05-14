// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy_Minimal.h"

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

	APlanetPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	void InitialData();
	
private:

	void UpdatePosition();

	UFUNCTION(NetMulticast, Reliable)
	void UpdateCurrentPosition(const FGameplayTag&NewCurrentRegionTag);
	
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent>AudioComponentPtr = nullptr;
	
	FString PlayerName;

	FGameplayTag CurrentRegionTag;
};
