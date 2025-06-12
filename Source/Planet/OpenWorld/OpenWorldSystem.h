 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "OpenWorldSystem.generated.h"

class ATargetPoint;
class ATeleport;
class APlanetPlayerController;

struct FTableRow_Teleport;

enum class ETeleport : uint8;

/*
 * 传送
 * Only Server
 */
UCLASS()
class PLANET_API UOpenWorldSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UOpenWorldSubSystem* GetInstance();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/**
	 * 获取距离离开开放世界时最近的传送点
	 * OnlyServer
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	ETeleport GetTeleportLastPtInOpenWorld(APlanetPlayerController* PCPtr);

	/**
	 * 获取最近的传送点
	 * OnlyServer
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	ETeleport GetTeleportPlayerToNearest(APlanetPlayerController* PCPtr);

	/**
	 * 根据传送点切换数据层
	 * OnlyServer
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	void SwitchDataLayer(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr);

	/**
	 * 确认切换的数据层中，必须的数据是否已存在
	 * OnlyServer
	 * @param ChallengeLevelType 
	 * @return 
	 */
	bool CheckSwitchDataLayerComplete(ETeleport ChallengeLevelType)const;

	/**
	 * 根据传送点切换数据层
	 * OnlyServer
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	void TeleportPlayer(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr);

	/**
	 * 根据传送点切换数据层
	 * OnlyServer
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	bool CheckTeleportPlayerComplete(ETeleport ChallengeLevelType)const;

	TSoftObjectPtr<ATeleport> GetTeleport(ETeleport ChallengeLevelType)const;

	ETeleport GetTeleportType(const TSoftObjectPtr<ATeleport>&TeleportPtr)const;

	FTableRow_Teleport* GetTeleportDT(ETeleport ChallengeLevelType)const;

	FGameplayTag GetTeleportWeather(ETeleport ChallengeLevelType)const;

	/**
	 * 根据指定挑战关卡和生成NPC数量 返回生成位置
	 * @param ChallengeLevelType 
	 * @param Num 
	 * @return 
	 */
	TArray<FTransform>GetChallengeSpawnPts(ETeleport ChallengeLevelType, int32 Num)const;
	
private:

};
