// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"

#include "ThreadHelper.h"

#include "UI/UIManagerSubSystem.h"

#include "PlanetGameMode.generated.h"

extern PLANET_API class AShiYuGameMode* GGameModePtr;

class UHoldingItemsComponent;
class UCharacterUnit;

/**
 *
 */
UCLASS()
class PLANET_API APlanetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	APlanetGameMode();
	
	UCharacterUnit* AddCharacterUnit(FGameplayTag UnitType);

	UCharacterUnit* FindCharacterUnit(int32 ID);

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
	UPROPERTY(Transient)
	TMap<int32, UCharacterUnit*> CharacterUnitMap;

};
