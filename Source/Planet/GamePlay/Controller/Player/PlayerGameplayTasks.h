// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTasksComponent.h"
#include "OpenWorldDataLayer.h"

#include "PlayerGameplayTasks.generated.h"

class ATeleport;
class APlanetPlayerController;

/*
 * PlayerController处理异步得组件，如
 * 传送
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

#pragma region WaitLoad
	
	/**
	 * Client
	 */
	void WaitPlayerLoad();

#pragma endregion
	
#pragma region Teleport

	/**
	 * Client
	 * 回到进入离开世界前的位置最近的传送点
	 */
	void TeleportPlayerToOpenWorld();

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

	/*
	 *	角色进入副本之前在开放世界的位置信息
	 */
	FTransform OpenWorldTransform = FTransform::Identity;
	
	/*
	 *	角色进入副本之前，开放世界的天气
	 */
	FGameplayTag OpenWorldWeather;
	
private:
	UFUNCTION(Server, Reliable)
	void TeleportPlayerToOpenWorld_Server();

	UFUNCTION(Server, Reliable)
	void TeleportPlayerToNearest_Server();

	UFUNCTION(Server, Reliable)
	void EntryChallengeLevel_Server(
		ETeleport Teleport
	);

	
	UFUNCTION(NetMulticast, Reliable)
	void EntryLevel_ActiveTask(
		ETeleport Teleport,
		const FGameplayTag &NewWeather
	);

	void EntryLevelEnd(
		bool bIsSuccess
	);
	
#pragma endregion
};

/**
 * 传送PlayeyController
 */
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

	FGameplayTag Weather;
	
	FOnEnd OnEnd;

private:
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

	TSoftObjectPtr<ATeleport> Target = nullptr;

	bool bIsSwitchDataLayerComplete = false;

	bool bIsSuccessful = false;

	bool bIsOnLanded = false;

	const int32 DistanceThreshold = 100;

	/**
	 * 最小运行时长
	 */
	int32 MinWaitTime = 1;
	
	/**
	 * 
	 */
	float CurrentWaitTime = 0.f;
};

/**
 * 等待游戏初始时加载任务完成
 * Only Client
 */
UCLASS()
class PLANET_API UGameplayTask_WaitLoadComplete : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(
		bool
	)>;

	UGameplayTask_WaitLoadComplete(
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

	FOnEnd OnEnd;

private:
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);
	
	bool bIsOnLanded = false;

	/**
	 * 最小运行时长
	 */
	int32 MinWaitTime = 1;
	
	/**
	 * 
	 */
	float CurrentWaitTime = 0.f;
};
