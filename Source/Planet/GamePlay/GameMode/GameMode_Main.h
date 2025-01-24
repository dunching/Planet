// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"



#include "UI/UIManagerSubSystem.h"

#include "GameMode_Main.generated.h"

extern PLANET_API class AShiYuGameMode* GGameModePtr;

class UInventoryComponent;
struct FCharacterProxy;

/**
 *
 */
UCLASS()
class PLANET_API AGameMode_Main : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGameMode_Main();
	
	FCharacterProxy* AddCharacterProxy(FGameplayTag ProxyType);

	FCharacterProxy* FindCharacterProxy(int32 ID);

protected:

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay()override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;

	virtual void BeginDestroy()override;

	UFUNCTION()
	void OnVoxelWorldGenerated();

	UFUNCTION()
	void OnVoxelWorldLoad();
	
protected:

private:

	void LoadGameImp();

	void SaveGameImp();

	FTimerHandle SaveGameTimer;

	FDelegateHandle Delegate;
	
	// 角色序列,第0个为Player
	TMap<int32, FCharacterProxy*> CharacterProxyMap;

};

UCLASS()
class PLANET_API APlanetGameMode : public AGameMode_Main
{
	GENERATED_BODY()
};