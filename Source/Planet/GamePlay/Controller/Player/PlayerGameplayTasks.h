// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTasksComponent.h"
#include "OpenWorldDataLayer.h"

#include "PlayerGameplayTasks.generated.h"

class ATeleport;
class APlanetPlayerController;

/*
 *
 */
UCLASS(BlueprintType, Blueprintable)
class PLANET_API UPlayerControllerGameplayTasksComponent : public UGameplayTasksComponent
{
	GENERATED_BODY()

public:
	using FOwnerType = APlanetPlayerController;

	static FName ComponentName;

	UPlayerControllerGameplayTasksComponent(
		const FObjectInitializer& ObjectInitializer
	);

#pragma region Teleport

	/**
	 * Client
	 */
	void TeleportPlayerToNearest();

	/**
	 * Client
	 */
	void EntryChallengeLevel(
		ETeleport Teleport
	);

private:
	UFUNCTION(Server, Reliable)
	void TeleportPlayerToNearest_Server();

	UFUNCTION(NetMulticast, Reliable)
	void TeleportPlayerToNearest_ActiveTask(
		ETeleport Teleport
	);

	void TeleportPlayerToNearestEnd(
		bool bIsSuccess
	);

	UFUNCTION(Server, Reliable)
	void EntryChallengeLevel_Server(
		ETeleport Teleport
	);

	UFUNCTION(NetMulticast, Reliable)
	void EntryChallengeLevel_ActiveTask(
		ETeleport Teleport
	);

	void EntryChallengeLevelEnd(
		bool bIsSuccess
	);

#pragma endregion
};

UCLASS()
class PLANET_API UGameplayTask_TeleportPlayer : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
	)>;

	UGameplayTask_TeleportPlayer(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;

	virtual void OnDestroy(
		bool bInOwnerFinished
	) override;

	TObjectPtr<APlanetPlayerController> TargetPCPtr = nullptr;

	ETeleport Teleport = ETeleport::kNone;

	FOnEnd OnEnd;

private:
	TSoftObjectPtr<ATeleport> Target = nullptr;

	bool bIsSwitchDataLayerComplete = false;

	bool bIsSuccessful = false;

	const int32 DistanceThreshold = 100;
};
