// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	ETeleport GetTeleportLastPtInOpenWorld(APlanetPlayerController* PCPtr);

	/**
	 * 获取最近的传送点
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	ETeleport GetTeleportPlayerToNearest(APlanetPlayerController* PCPtr);

	/**
	 * 根据传送点切换数据层
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	void SwitchDataLayer(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr);

	/**
	 * 确认切换的数据层中，必须的数据是否已存在
	 * @param ChallengeLevelType 
	 * @return 
	 */
	bool CheckSwitchDataLayerComplete(ETeleport ChallengeLevelType)const;

	/**
	 * 根据传送点切换数据层
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	void TeleportPlayer(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr);

	/**
	 * 根据传送点切换数据层
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	bool CheckTeleportPlayerComplete(ETeleport ChallengeLevelType)const;

	TSoftObjectPtr<ATeleport> GetTeleport(ETeleport ChallengeLevelType)const;

	/**
	 * 根据指定挑战关卡和生成NPC数量 返回生成位置
	 * @param ChallengeLevelType 
	 * @param Num 
	 * @return 
	 */
	TArray<FTransform>GetChallengeSpawnPts(ETeleport ChallengeLevelType, int32 Num)const;
	
private:

	/*
	 *	角色进入副本之前在开放世界的位置信息
	 */
	FTransform OpenWorldTransform = FTransform::Identity;
	
};
