// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GroupManaggerBase.h"

#include "PlanetAbilitySystemComponent.h"

#include "GroupManagger.generated.h"

class APlanetPlayerController;
class AHumanCharacter;
class ACharacterBase;
class IPlanetControllerInterface;
class AGroupManagger;
struct FSkillProxy;
struct FActiveSkillProxy;
struct FConsumableProxy;
struct FCharacterProxy;
class UPlanetAbilitySystemComponent;
class UGSAbilitySystemComponent;
class UProxySycHelperComponent;
class UTeamMatesHelperComponent;
class UInventoryComponent;
struct FSceneProxyContainer;

/*
 *	对局内每个组群共享的资源
 *	
 *	例如每个Player控制的Pawn和AI队友、群落AI
 *	一个组的持有的物品的信息
 *
 *	每个PlayerController会拥有一个，并于AI队友共享数据
 *	
 *	每组NPC会共用一个，由AGeneratorBase生成
 *
 *	单独的NPC会自己生成（SpawnDefaultController）一个，如功能性的NPC
 */
UCLASS()
class PLANET_GROUPMANAGGER_API AGroupManagger:
	public AGroupManaggerBase
{
	GENERATED_BODY()

public:

	AGroupManagger(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	virtual UPlanetAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTeamMatesHelperComponent* GetTeamMatesHelperComponent();

	UInventoryComponent* GetInventoryComponent();

	void SetOwnerCharacterProxyPtr(ACharacterBase*CharacterPtr);
	
	FGuid GroupID;
	
protected:
	
};
