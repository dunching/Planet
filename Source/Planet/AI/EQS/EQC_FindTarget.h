// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <EnvironmentQuery/EnvQueryContext.h>

#include "EQC_FindTarget.generated.h"

UCLASS()
class PLANET_API UEQC_Test : public UEnvQueryContext
{
	GENERATED_BODY()

public:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};

/**
 * 获取敌人列表里最近的敌人
 */
UCLASS()
class PLANET_API UEQC_GetTarget : public UEnvQueryContext
{
	GENERATED_BODY()

public:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};

/**
 * 获取玩家锁定的敌人
 */
UCLASS()
class PLANET_API UEQC_GetPlayerFocus : public UEnvQueryContext
{
	GENERATED_BODY()

public:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};

/**
 * 获取玩家位置
 * NPC跟随玩家时
 */
UCLASS()
class PLANET_API UEQC_GetLeader : public UEnvQueryContext
{
	GENERATED_BODY()

public:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};