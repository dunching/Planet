// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"

#include "GenerateType.h"

#include "PlayerComponent.generated.h"

class USceneComponent;
class AHumanCharacter_Player;

/**
 *
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class PLANET_API UPlayerComponent :
	public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = AHumanCharacter_Player;

	static FName ComponentName;
	
	UPlayerComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	virtual bool TeleportTo(
		const FVector& DestLocation,
		const FRotator& DestRotation,
		bool bIsATest=false,
		bool bNoCheck=false
		);
	
};