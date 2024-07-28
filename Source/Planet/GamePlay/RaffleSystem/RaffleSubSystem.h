// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "UICommon.h"
#include "RaffleCommon.h"
#include "SceneElement.h"
#include "SceneUnitExtendInfo.h"

#include "RaffleSubSystem.generated.h"

class UBasicUnit;

UCLASS()
class PLANET_API URaffleSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	using FOnGetUnitAry = 
		TCallbackHandleContainer<void(const TArray<TPair<FTableRowUnit, TSubclassOf<UBasicUnit>>>&)>;

	static URaffleSubSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	bool Raffle(ERaffleType RaffleType, int32 Count)const;

	void SyncUnits2Player()const;

	FOnGetUnitAry OnGetUnitAry;

protected:

	bool RafflePermanent(int32 Count)const;

	void RafflePermanentComplete(
#if WITH_EDITOR
		int32 Count
#endif
	)const;

	int32 CostPerRaffle = 180;

	FGuid ApendingID = FGuid::NewGuid();

};
