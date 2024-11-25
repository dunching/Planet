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
class UTeamConfigureComponent;
class UTeamMatesHelperComponent;
struct FSceneUnitContainer;

/*
 *	贡献的资源
 *	一个组的持有的物品的信息
 */
UCLASS()
class PLANET_API AGroupSharedInfo: public AInfo
{
	GENERATED_BODY()

public:

	AGroupSharedInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UTeamMatesHelperComponent* GetTeamMatesHelperComponent();

	UPROPERTY()
	TObjectPtr<UTeamMatesHelperComponent> TeamMatesHelperComponentPtr = nullptr;
	
	FGuid GroupID;
	
};
