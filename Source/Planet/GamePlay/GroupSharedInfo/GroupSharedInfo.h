// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenerateType.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy.h"

#include "GroupSharedInfo.generated.h"

class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
class AGroupSharedInfo;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
class UPlanetAbilitySystemComponent;
class UProxySycHelperComponent;
class UTeamMatesHelperComponent;
class UHoldingItemsComponent;
struct FSceneUnitContainer;

/*
 *	对局内每个组群共享的资源
 *	例如每个Player控制的Pawn和AI队友、群落AI
 *	一个组的持有的物品的信息
 */
UCLASS()
class PLANET_API AGroupSharedInfo: public AInfo
{
	GENERATED_BODY()

public:

	AGroupSharedInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UTeamMatesHelperComponent* GetTeamMatesHelperComponent();

	UHoldingItemsComponent* GetHoldingItemsComponent();

	FGuid GroupID;
	
protected:
	
	UPROPERTY()
	TObjectPtr<UTeamMatesHelperComponent> TeamMatesHelperComponentPtr = nullptr;
	
	UPROPERTY()
	TObjectPtr<UHoldingItemsComponent> HoldingItemsComponentPtr = nullptr;
	
};
