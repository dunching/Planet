// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PlayerGameplayTasks.generated.h"

class ATeleport;
class APlanetPlayerController;

UCLASS()
class PLANET_API UGameplayTask_TeleportPlayer : public UGameplayTask
{
	GENERATED_BODY()

public:
	using FOnEnd = TMulticastDelegate<void(bool)>;

	UGameplayTask_TeleportPlayer(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void Activate() override;

	virtual void TickTask(
		float DeltaTime
	) override;
	
	virtual void OnDestroy(bool bInOwnerFinished)override;
	
	TObjectPtr<APlanetPlayerController> TargetPCPtr = nullptr;

	const int32 DistanceThreshold = 100;

	FOnEnd OnEnd;

private:
	TSoftObjectPtr<ATeleport> Target = nullptr;

	bool bIsSuccessful = false;
};
