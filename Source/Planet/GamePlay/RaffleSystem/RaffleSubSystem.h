// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "UICommon.h"
#include "RaffleCommon.h"

#include "RaffleSubSystem.generated.h"

UCLASS()
class PLANET_API URaffleSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static URaffleSubSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	bool Raffle(ERaffleType RaffleType, int32 Count)const;

};
