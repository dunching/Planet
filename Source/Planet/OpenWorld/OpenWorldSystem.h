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
	 * 将Player传送至该点
	 * @param ChallengeLevelType 
	 * @param PCPtr 
	 */
	void SwitchDataLayer(ETeleport ChallengeLevelType, APlanetPlayerController* PCPtr);

	ETeleport GetTeleportPlayerToNearest(APlanetPlayerController* PCPtr);

	/**
	 * 确认切换的数据层中，必须的数据是否已存在
	 * @param ChallengeLevelType 
	 * @return 
	 */
	bool CheckSwitchDataLayerComplete(ETeleport ChallengeLevelType);

	TSoftObjectPtr<ATeleport> GetTeleport(ETeleport ChallengeLevelType)const;

	/**
	 * 根据指定挑战关卡和生成NPC数量 返回生成位置
	 * @param ChallengeLevelType 
	 * @param Num 
	 * @return 
	 */
	TArray<FTransform>GetChallengeSpawnPts(ETeleport ChallengeLevelType, int32 Num)const;
	
private:

	void SwitchDataLayerImp(
		const FName& Key,
		const FTableRow_Teleport& Value,
		ETeleport ChallengeLevelType
		);

	void TeleportPlayer(
		const FName& Key,
		const FTableRow_Teleport& Value,
		ETeleport ChallengeLevelType,
		APlanetPlayerController* PCPtr
		);

	/*
	 *	角色进入副本之前在开放世界的位置信息
	 */
	FTransform OpenWorldTransform = FTransform::Identity;
	
};
